#include "Server.hpp"

#include <ixwebsocket/IXWebSocket.h>

#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

Server::Server(const std::shared_ptr<CommunicationService>& communication, int port, const std::string& host)
    : server(std::make_unique<ix::WebSocketServer>(port, host)), logger(NoOptLogger::getInstance()), communication(communication)
{
	logger->info("Server created");
	server = std::make_unique<ix::WebSocketServer>(port, host);
	auto [success, error] = server->listen();

	if (!success) logger->error("Failed to start the server: {}", error);

	// Set up connection and message handlers
	server->setOnConnectionCallback(std::bind(&Server::onConnection, this, std::placeholders::_1, std::placeholders::_2));

	logger->info("Server created");

	communication->registerCommandHandler(Push(), std::bind(&Server::push, this, std::placeholders::_1));
}

void Server::start()
{
	logger->info("Starting the WebsocketService");
	server->start();
	server->wait();
}

void Server::onConnection(const std::weak_ptr<ix::WebSocket>& webSocket, const std::shared_ptr<ix::ConnectionState>& connectionState)
{
	auto ws = webSocket.lock();

	if (ws) ws->setOnMessageCallback(std::bind(&Server::onWebSocketMessage, this, ws, std::placeholders::_1, connectionState));
}

void Server::onWebSocketMessage(const std::shared_ptr<ix::WebSocket>& webSocket, const ix::WebSocketMessagePtr& message,
                                const std::shared_ptr<ix::ConnectionState>& connectionState)
{
	switch (message->type)
	{
		case ix::WebSocketMessageType::Open:
		{
			std::lock_guard<std::mutex> lock(connectionMutex);

			std::string connectionId = connectionState->getId();
			std::string userId = message->openInfo.uri.substr(1);
			CommandResult game = communication->execute(std::make_shared<GetRoomByPlayerId>(userId));

			connections[connectionState->getId()] = webSocket;
			players[userId] = connectionId;
			rooms[game["id"].get<std::string>()].push_back(connectionId);

			logger->info("Found room: {}", game.dump());

			break;
		}
		case ix::WebSocketMessageType::Close:
		{
			std::lock_guard<std::mutex> lock(connectionMutex);
			std::string connectionId = connectionState->getId();

			auto playerIt = std::find_if(players.begin(), players.end(), [&connectionId](auto&& p) { return p.second == connectionId; });
			if (playerIt != players.end()) players.erase(playerIt);

			auto roomIt = std::find_if(rooms.begin(), rooms.end(), [&connectionId](auto&& r) { return std::find(r.second.begin(), r.second.end(), connectionId) != r.second.end(); });
			if (roomIt != rooms.end()) roomIt->second.erase(std::find(roomIt->second.begin(), roomIt->second.end(), connectionId));

			connections.erase(connectionState->getId());
			
			break;
		}
		case ix::WebSocketMessageType::Message:
			handleMessage(webSocket, message);

			webSocket->send(std::format("Ping: {}", message->str));
			break;
		case ix::WebSocketMessageType::Error:
		case ix::WebSocketMessageType::Ping:
		case ix::WebSocketMessageType::Pong:
		case ix::WebSocketMessageType::Fragment:
			break;
	}
}

void Server::handleMessage(const std::shared_ptr<ix::WebSocket>& webSocket, const ix::WebSocketMessagePtr& message)
{
	logger->info("Received message: {}", message->str);

	nlohmann::json data = nlohmann::json::parse(message->str);

	if (data.value("type", "") == "response")
	{
	}
	else
	{
		logger->info("Executing command: {}", data.dump());
		CommandResult result = communication->execute(std::make_shared<Command>(data));

		if (!result.empty())
		{
			logger->info("Sending response: {}", result.dump());
			webSocket->send(result.dump());
		}
		else { logger->info("No response to send"); }
	}
}

CommandResult Server::push(const nlohmann::json& command) const
{
	const PushData data = static_cast<Push>(command).data;

	switch (data.target) {
		case PushTarget::ROOM:
			if (!rooms.contains(data.receiver))
			{
				logger->error("Room {} not found", data.receiver);
				return CommandResult();
			}

			for (const auto& player : rooms.at(data.receiver))
			{
				auto socket = connections.at(player);
				socket->send(data.message.dump());
			}

			break;
		case PushTarget::PLAYER:
			if (!players.contains(data.receiver))
			{
				logger->error("Player {} not found", data.receiver);
				return CommandResult();
			}

			auto socket = connections.at(players.at(data.receiver));
			socket->send(data.message.dump());
			break;
	}

	return CommandResult();
}
