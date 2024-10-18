#pragma once

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

using communication::Command;
using communication::CommandResult;

using namespace communication::commands;

class Server
{
public:
	Server(const std::shared_ptr<CommunicationService>& communication, int port = 8008, const std::string& host = "0.0.0.0");

	void start();

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

private:
	std::unique_ptr<ix::WebSocketServer> server;
	std::shared_ptr<Logger> logger;
	std::shared_ptr<CommunicationService> communication;

	std::unordered_map<std::string, std::vector<std::string>> rooms;
	std::unordered_map<std::string, std::string> players;
	std::unordered_map<std::string, std::shared_ptr<ix::WebSocket>> connections;
	std::mutex connectionMutex;

	void onConnection(const std::weak_ptr<ix::WebSocket>& webSocket, const std::shared_ptr<ix::ConnectionState>& connectionState);
	void onWebSocketMessage(const std::shared_ptr<ix::WebSocket>& webSocket, const ix::WebSocketMessagePtr& message, const std::shared_ptr<ix::ConnectionState>& connectionStat);

	void handleMessage(const std::shared_ptr<ix::WebSocket>& webSocket, const ix::WebSocketMessagePtr& message);

	CommandResult push(const nlohmann::json& command) const;
};
