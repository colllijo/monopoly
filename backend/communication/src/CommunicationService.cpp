#include "CommunicationService.hpp"

#include <amqpcpp/flags.h>
#include <amqpcpp/linux_tcp/tcpconnection.h>

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

#include "nlohmann/json_fwd.hpp"

CommunicationService::CommunicationService()
    : base(event_base_new(), event_base_free), handler(AMQP::LibEventHandler(base.get()))
{
	if (!base) throw std::runtime_error("Failed to create event base");

	connection = std::make_unique<AMQP::TcpConnection>(&handler, AMQP::Address("amqp://guest:guest@localhost"));
	channel = std::make_unique<AMQP::TcpChannel>(connection.get());

	// Declare queues
	channel->declareQueue("game_queue");
	channel->declareQueue("data_queue");

	std::cout << "Connected to RabbitMQ" << std::endl;
};

void CommunicationService::start()
{
	communicationThread = std::thread([this]() { event_base_dispatch(base.get()); });
}

void CommunicationService::stop()
{
	if (channel->connected()) channel->close();
	if (connection->ready()) connection->close();

	event_base_loopbreak(base.get());

	if (communicationThread.joinable()) communicationThread.join();
	communicationThread.detach();
}

nlohmann::json CommunicationService::execute(const amqp::Command &command)
{
	nlohmann::json json = command;
	std::string payload = json.dump();
	std::string correlationId = createCorrelationId();
	std::string queue;

	switch (command.queue) {
		case amqp::CommandQueue::GAME:
			queue = "game_queue";
			break;
		case amqp::CommandQueue::DATA:
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
		std::cerr << "Failed to parse response for " << correlationId << ": " << e.what() << std::endl;
		responses.put(message.correlationID(), nullptr);
	}

	requests.erase(message.correlationID());
	channel->removeQueue(message.routingkey());
}
