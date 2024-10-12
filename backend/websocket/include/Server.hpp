#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

#include "communication/Logger.hpp"

class Server
{
public:
	Server(int port = 8008, const std::string& host = "0.0.0.0");

	void start();

	void setLogger(std::shared_ptr<Logger> logger) { this->logger = logger; }

private:
	std::unique_ptr<ix::WebSocketServer> server;
	std::shared_ptr<Logger> logger;

	std::unordered_map<std::string, ix::WebSocket> connections;
};
