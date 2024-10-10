#include "CommunicationService.hpp"

#include <chrono>
#include <mutex>

CommunicationService::CommunicationService(const std::shared_ptr<Logger> logger)
    : logger(logger),
      base(event_base_new(), event_base_free),
      async(false),
      handler(AMQP::LibEventHandler(base.get())),
      commandQueue(std::bind(&CommunicationService::handleCommand, this, std::placeholders::_1)),
      messageQueue(std::bind(&CommunicationService::handleMessage, this, std::placeholders::_1))
{
	if (!base) throw std::runtime_error("Failed to create event base");

	const char *broker = std::getenv("BROKER");
	if (!broker) broker = "localhost";

	connection = std::make_unique<AMQP::TcpConnection>(&handler, AMQP::Address(std::format("amqp://{}", broker)));
	channel = std::make_unique<AMQP::TcpChannel>(connection.get());
	channel->setQos(4);

	// Declare queues
	for (const auto &[queue, queueName] : communication::QUEUE_NAMES) declareQueue(queueName);
	declareResponseQueue();
};

void CommunicationService::declareQueue(const std::string_view &queueName)
{
	channel->declareQueue(queueName, AMQP::durable)
	    .onSuccess(
	        [this, queueName](const std::string &name, int, int)
	        {
		        channel->consume(name)
		            .onReceived(
		                [this](const AMQP::Message &message, uint64_t deliveryTag, bool)
		                {
			                // Check if the message is valid
			                if (!message.hasReplyTo() || !message.hasCorrelationID())
			                {
				                channel->ack(deliveryTag);
				                return;
			                }

			                // Parse the command
			                Command command;
			                try
			                {
				                command = nlohmann::json::parse(std::string(message.body(), message.bodySize()));
			                }
			                catch (const std::exception &e)
			                {
				                channel->ack(deliveryTag);
				                return;
			                }

							// Check if command is expired
							std::unique_lock<std::mutex> lock(requestsMutex);
							if (expired.contains(message.correlationID()))
							{
								channel->ack(deliveryTag);
								expired.erase(message.correlationID());
								lock.unlock();
								return;
							}
							lock.unlock();

			                // Check if the command is registered
			                if (!handlers.contains(command))
			                {
				                channel->reject(deliveryTag, AMQP::requeue);
				                return;
			                }

			                channel->ack(deliveryTag);
			                logger->info("Received command: {}", command.command);
			                commandQueue.push({command, message.correlationID(), message.replyTo(), deliveryTag});
		                })
		            .onSuccess([this, queueName]() { logger->info("Listening on queue: {}", queueName); })
		            .onError([this, queueName](const char *message) { logger->error("Failed to consume from queue {}: {}", queueName, message); });
	        });
}

void CommunicationService::declareResponseQueue()
{
	channel->declareQueue(AMQP::exclusive)
	    .onSuccess(
	        [this](const std::string &name, int, int)
	        {
		        responseQueue = name;
		        logger->info("Created exclusive queue: {}", name);
		        channel->consume(name, AMQP::noack)
		            .onReceived(
		                [this](const AMQP::Message &message, uint64_t deliveryTag, bool)
		                {
			                // Check if the message is valid
			                if (!message.hasCorrelationID()) return;

			                // Parse the message
			                CommandResult result;
			                try
			                {
				                result = nlohmann::json::parse(std::string(message.body(), message.bodySize()));
			                }
			                catch (const std::exception &e)
			                {
				                return;
			                }

			                logger->info("Received message: {}", result.dump());
			                messageQueue.push({std::string(message.body(), message.bodySize()), message.correlationID(), deliveryTag});
		                })
		            .onSuccess([this, name]() { logger->info("Listening on exclusive queue: {}", name); })
		            .onError([this](const char *message) { logger->error("Failed to consume from exclusive queue: {}", message); });
	        })
	    .onError([this](const char *message) { logger->error("Failed to create exclusive queue: {}", message); });
}

void CommunicationService::start()
{
	logger->info("CommunicationService started");
	channel->onReady([this]() { logger->info("Connected to RabbitMQ broker"); });

	event_base_dispatch(base.get());
}

void CommunicationService::start_async()
{
	async = true;
	communicationThread = std::thread([this]() { start(); });
}

void CommunicationService::stop()
{
	if (channel->connected()) channel->close();
	if (connection->ready()) connection->close();

	event_base_loopbreak(base.get());

	if (async && communicationThread.joinable()) communicationThread.join();

	logger->info("CommunicationService stopped");
}

void CommunicationService::registerCommandHandler(const Command &command, CommandCallback callback) { handlers[command] = callback; }

CommandResult CommunicationService::execute(const Command &command)
{
	std::shared_ptr<CommandPromise> promise = std::make_shared<CommandPromise>();
	std::future<CommandResult> future = promise->get_future();

	std::string correlationId = createCorrelationId();

	{
		std::lock_guard<std::mutex> lock(requestsMutex);
		requests[correlationId] = std::move(promise);
	}

	std::string payload = ((nlohmann::json)command).dump();

	AMQP::Envelope msg(payload.c_str(), payload.size());
	msg.setReplyTo(responseQueue);
	msg.setCorrelationID(correlationId);

	channel->publish("", communication::getQueuenName(command.queue), msg);

	if (future.wait_for(std::chrono::seconds(8)) == std::future_status::ready) return future.get();

	logger->info("Command timed out: {}", command.command);
	removeCommand(correlationId);
	return nullptr;
}

void CommunicationService::removeCommand(const std::string &correlationId)
{
	std::unique_lock<std::mutex> lock(requestsMutex);
	requests.erase(correlationId);
	expired.insert(correlationId);

	lock.unlock();
}

void CommunicationService::handleCommand(const CommandData &data)
{
	if (!handlers.contains(data.command)) return;
	std::string payload = (handlers.at(data.command)(data.command)).dump();

	AMQP::Envelope response(payload.c_str(), payload.size());
	response.setCorrelationID(data.correlationId);

	channel->publish("", data.replyTo, response);
}

void CommunicationService::handleMessage(const MessageData &data)
{
	if (!requests.contains(data.correlationId)) return;

	std::unique_lock<std::mutex> lock(requestsMutex);
	std::shared_ptr<std::promise<CommandResult>> promise = std::move(requests.at(data.correlationId));
	std::string response(data.message);

	try
	{
		promise->set_value(nlohmann::json::parse(response));
	}
	catch (const nlohmann::json::parse_error &e)
	{
		logger->error("Failed to parse response for {}: {}", data.correlationId, e.what());
		promise->set_value(nullptr);
	}

	requests.erase(data.correlationId);
	lock.unlock();
}
