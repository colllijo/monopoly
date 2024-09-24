#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <event2/event.h>

#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

#include <unordered_set>
#include <uuid/uuid.h>

#include "Command.hpp"
#include "ThreadSafeMap.hpp"
#include "nlohmann/json_fwd.hpp"

class CommunicationService
{
public:
	using EventBasePtr = std::unique_ptr<struct event_base, std::function<void(struct event_base*)>>;
	using EventPtr = std::unique_ptr<struct event, std::function<void(struct event*)>>;

	using CommandCallback = std::function<nlohmann::json(communication::Command)>;

	CommunicationService();

	void start();
	void start_async();
	void stop();

	nlohmann::json execute(const struct communication::Command& command);
	void handleCommand(const communication::Command &command, CommandCallback callback);

private:
	EventBasePtr base;
	std::thread communicationThread;

	bool async;

	AMQP::LibEventHandler handler;
	std::unique_ptr<AMQP::TcpConnection> connection;
	std::unique_ptr<AMQP::TcpChannel> channel;

	std::unordered_set<std::string> requests;
	ThreadSafeMap<std::string, nlohmann::json> responses;
	std::map<communication::Command, CommandCallback> handlers;

	void declareQueue(const std::string& name);

	void onMessage(const AMQP::Message& message, uint64_t deliveryTag, bool redelivered);
	void onCommand(communication::Command);

	std::string createCorrelationId() const {
		uuid_t uuid;
		char uuid_str[37];

		uuid_generate(uuid);
		uuid_unparse(uuid, uuid_str);

		return std::string(uuid_str);
	};
};
