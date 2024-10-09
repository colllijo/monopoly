#pragma once

#include <amqpcpp.h>
#include <amqpcpp/libevent.h>
#include <amqpcpp/message.h>
#include <event2/event.h>
#include <uuid/uuid.h>

#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#include "AsyncEventQueue.hpp"
#include "Command.hpp"
#include "Logger.hpp"
#include "NoOptLogger.hpp"

using communication::Command;
using communication::CommandQueue;
using communication::CommandResult;

struct CommandData
{
	Command command;
	std::string correlationId;
	std::string replyTo;
	uint64_t deliveryTag;
};

struct MessageData
{
	std::string message;
	std::string correlationId;
	uint64_t deliveryTag;
};

class CommunicationService
{
public:
	using EventBasePtr = std::unique_ptr<struct event_base, std::function<void(struct event_base*)>>;
	using EventPtr = std::unique_ptr<struct event, std::function<void(struct event*)>>;
	using CommandCallback = std::function<CommandResult(const communication::Command&)>;
	using CommandPromise = std::promise<CommandResult>;

	CommunicationService(const std::shared_ptr<Logger> logger = NoOptLogger::getInstance());
	~CommunicationService() = default;

	void start();
	void start_async();
	void stop();

	CommandResult execute(const struct communication::Command& command);
	void handleCommand(const communication::Command& command, CommandCallback callback);

	void setLogger(const std::shared_ptr<Logger> logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	EventBasePtr base;

	bool async;
	std::thread communicationThread;

	AMQP::LibEventHandler handler;
	std::unique_ptr<AMQP::TcpConnection> connection;
	std::unique_ptr<AMQP::TcpChannel> channel;
	std::string responseQueue;

	std::map<communication::Command, CommandCallback> handlers;
	std::unordered_map<std::string, std::shared_ptr<CommandPromise>> requests;
	std::mutex requestsMutex;

	AsyncEventQueue<CommandData> commandQueue;
	AsyncEventQueue<MessageData> messageQueue;

	void declareQueue(const std::string_view& name);
	void declareResponseQueue();

	void handleWork(const CommandData& data);
	void handleMessage(const MessageData& data);

	std::string createCorrelationId() const
	{
		uuid_t uuid;
		char uuid_str[37];

		uuid_generate(uuid);
		uuid_unparse(uuid, uuid_str);

		return std::string(uuid_str);
	};
};
