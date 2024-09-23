#pragma once

#include <crow.h>
#include <crow/http_request.h>

#include <memory>
#include <string>
#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"

class GameController
{
public:
	GameController(std::shared_ptr<crow::SimpleApp> app, std::shared_ptr<CommunicationService> communication);

private:
	std::shared_ptr<crow::SimpleApp> app;
	std::shared_ptr<CommunicationService> communication;

	constexpr static amqp::Command GetGamesCommand = {"get_games", amqp::CommandQueue::GAME};

	// Method to register routes
	void registerRoutes();

	// Route handler methods
	crow::response handleGetGames();
	crow::response handleCreateGame(const crow::request &req);
	crow::response handleJoinGame(std::string gameId);
	crow::response handleLeaveGame(std::string gameId);
};

