#include "Server.hpp"

#include "communication/NoOptLogger.hpp"

Server::Server(int port, const std::string& host) : server(std::make_unique<ix::WebSocketServer>(port, host)), logger(NoOptLogger::getInstance())
{
	server = std::make_unique<ix::WebSocketServer>(port, host);
	auto [success, error] = server->listen();

	if (!success) logger->error("Failed to start the server: {}", error);

	server->setOnClientMessageCallback(
	    [this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket& webSocket, const ix::WebSocketMessagePtr& msg)
	    {
			logger->info("Remote ip: {}", connectionState->getRemoteIp());

		    if (msg->type == ix::WebSocketMessageType::Open)
		    {
				logger->info("New connection");
				logger->info("\tid: {}", connectionState->getId());
				logger->info("\tUri: {}", msg->openInfo.uri);
				logger->info("\tHeaders:");
			    for (auto it : msg->openInfo.headers)
					logger->info("\t\t{}: {}", it.first, it.second);
		    }
		    else if (msg->type == ix::WebSocketMessageType::Message)
		    {
				logger->info("Received: {}", msg->str);

			    webSocket.send(msg->str, msg->binary);
		    }
	    });
}

void Server::start()
{
	server->start();
	server->wait();
}
