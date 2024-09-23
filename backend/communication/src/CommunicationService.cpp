#include "CommunicationService.hpp"

#include <amqpcpp/flags.h>
#include <amqpcpp/linux_tcp/tcpconnection.h>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include "nlohmann/json_fwd.hpp"

CommunicationService::CommunicationService()
	: base(event_base_new(), event_base_free)
	, handler(AMQP::LibEventHandler(base.get()))
{
	if (!base) throw std::runtime_error("Failed to create event base");

	connection = std::make_unique<AMQP::TcpConnection>(&handler, AMQP::Address("amqp://guest:guest@localhost"));
	channel = std::make_unique<AMQP::TcpChannel>(connection.get());

	declareQueues();

	std::cout << "Connected to RabbitMQ" << std::endl;
};

void CommunicationService::start()
{
	communicationThread = std::thread([this]() {
		event_base_dispatch(base.get());
	});
}

void CommunicationService::stop()
{
	if (channel->connected()) channel->close();
	if (connection->ready()) connection->close();

	event_base_loopbreak(base.get());

	if (communicationThread.joinable()) communicationThread.join();
	communicationThread.detach();
}

void CommunicationService::declareQueues()
{
	channel->declareQueue("game_queue");
	channel->declareQueue("data_queue");
}

nlohmann::json CommunicationService::execute(const amqp::Command& command)
{
	nlohmann::json json = command;
	std::string payload = json.dump();

	channel->declareQueue(AMQP::exclusive).onSuccess([this, payload](const std::string &name, int, int) {
		AMQP::Envelope msg(payload.c_str(), payload.size());
		msg.setReplyTo(name);

		channel->publish("", "data_queue", msg);
		channel->consume(name).onReceived([this](const AMQP::Message &message, uint64_t, bool) {
			std::string response(message.body(), message.bodySize());
			std::cout << "Received: " << response << std::endl;

			responses.put("games", nlohmann::json::parse(response));
		});
	});

	std::optional<nlohmann::json> response = responses.get("games");

	if (response.has_value()) return response.value();
	return nullptr;
}
