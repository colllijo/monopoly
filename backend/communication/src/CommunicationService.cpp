#include "CommunicationService.hpp"

#include <amqpcpp/flags.h>
#include <amqpcpp/linux_tcp/tcpconnection.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "Command.hpp"
#include "Logger.hpp"

CommunicationService::CommunicationService(const std::shared_ptr<Logger> logger)
    : logger(logger), base(event_base_new(), event_base_free), async(false), handler(AMQP::LibEventHandler(base.get()))
{
	if (!base) throw std::runtime_error("Failed to create event base");

	connection = std::make_unique<AMQP::TcpConnection>(&handler, AMQP::Address("amqp://guest:guest@localhost"));
	channel = std::make_unique<AMQP::TcpChannel>(connection.get());

	// Declare queues
	declareQueue("game_queue");
	declareQueue("data_queue");
};

void CommunicationService::declareQueue(const std::string &queueName)
{
	channel->declareQueue(queueName, AMQP::durable).onSuccess([this, queueName](const std::string &name, int, int) {
		channel->consume(name).onReceived([this](const AMQP::Message &message, uint64_t deliveryTag, bool) {
			if (!message.hasReplyTo()) {
				channel->reject(deliveryTag, AMQP::requeue);
				return;
			}

			communication::Command command;

			try {
				nlohmann::json json = nlohmann::json::parse(std::string(message.body(), message.bodySize()));
				command = json;
			} catch (const nlohmann::json::parse_error &e) {
				logger->error("Failed to parse message: {}", e.what());
				channel->reject(deliveryTag);
				return;
			}

			if (!handlers.contains(command)) {
				channel->reject(deliveryTag, AMQP::requeue);
				return;
			}

			std::string payload = (handlers.at(command)(command)).dump();

			AMQP::Envelope msg(payload.c_str(), payload.size());
			msg.setCorrelationID(message.correlationID());

			channel->ack(deliveryTag);
			channel->startTransaction();
			channel->publish("", message.replyTo(), msg);
			
			channel->commitTransaction();
		});
	});
}

void CommunicationService::start()
{
	logger->info("CommunicationService started");

	event_base_dispatch(base.get());
}

void CommunicationService::start_async() {
	async = true;
	communicationThread = std::thread([this]() { start(); });
}

void CommunicationService::stop()
{
	if (channel->connected()) channel->close();
	if (connection->ready()) connection->close();

	event_base_loopbreak(base.get());

	if (async) {
		if (communicationThread.joinable()) communicationThread.join();
		else communicationThread.detach();
	}

	logger->info("CommunicationService stopped");
}

void CommunicationService::handleCommand(const communication::Command &command, CommandCallback callback)
{
	handlers[command] = callback;
}

nlohmann::json CommunicationService::execute(const communication::Command &command)
{
	nlohmann::json json = command;
	std::string payload = json.dump();
	std::string correlationId = createCorrelationId();
	std::string queue;

	switch (command.queue) {
		case communication::CommandQueue::GAME:
			queue = "game_queue";
			break;
		case communication::CommandQueue::DATA:
			queue = "data_queue";
			break;
	}


	channel->declareQueue(AMQP::exclusive)
	    .onSuccess(
	        [this, correlationId, payload, queue](const std::string &name, int, int)
	        {
		        AMQP::Envelope msg(payload.c_str(), payload.size());
		        msg.setReplyTo(name);
		        msg.setCorrelationID(correlationId);

		        requests.insert(correlationId);

		        channel->publish("", queue, msg);
		        channel->consume(name).onReceived(std::bind(&CommunicationService::onMessage, this,
		                                                    std::placeholders::_1, std::placeholders::_2,
		                                                    std::placeholders::_3));
	        });

	std::optional<nlohmann::json> response = responses.get(correlationId);

	if (response.has_value()) return response.value();
	return nullptr;
}

void CommunicationService::onMessage(const AMQP::Message &message, uint64_t, bool)
{
	if (!message.hasCorrelationID() || !requests.contains(message.correlationID())) return;

	std::string correlationId = message.correlationID();
	std::string response(message.body(), message.bodySize());

	try {
		responses.put(message.correlationID(), nlohmann::json::parse(response));
	} catch (const nlohmann::json::parse_error &e) {
		logger->error("Failed to parse response for {}: {}", correlationId, e.what());
		responses.put(message.correlationID(), nullptr);
	}

	requests.erase(message.correlationID());
	channel->removeQueue(message.routingkey());
}
