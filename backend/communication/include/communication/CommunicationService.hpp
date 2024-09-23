#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
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

	CommunicationService();

	void start();
	void stop();

	nlohmann::json execute(const struct amqp::Command& command);

private:
	EventBasePtr base;
	std::thread communicationThread;

	AMQP::LibEventHandler handler;
	std::unique_ptr<AMQP::TcpConnection> connection;
	std::unique_ptr<AMQP::TcpChannel> channel;

	std::unordered_set<std::string_view> requests;
	ThreadSafeMap<std::string, nlohmann::json> responses;

	void onMessage(const AMQP::Message& message, uint64_t deliveryTag, bool redelivered);

	std::string createCorrelationId() const {
		uuid_t uuid;
		char uuid_str[37];

		uuid_generate(uuid);
		uuid_unparse(uuid, uuid_str);

		return std::string(uuid_str);
	};
};
