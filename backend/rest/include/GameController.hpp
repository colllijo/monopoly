#pragma once

#include <crow/app.h>

#include <memory>
#include <string>

#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"
#include "crow/middlewares/cors.h"

class GameController
{
public:
	GameController(const std::shared_ptr<crow::App<crow::CORSHandler>>& app, const std::shared_ptr<CommunicationService>& communication);

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<crow::App<crow::CORSHandler>> app;
	std::shared_ptr<CommunicationService> communication;

	// Method to register routes
	void registerRoutes() const;

	// Route handler methods
	crow::response handleGetGames() const;
	crow::response handleCreateGame(const crow::request &req) const;
	crow::response handleJoinGame(const crow::request &req, std::string &gameId) const;
	crow::response handleLeaveGame(std::string &gameId) const;
	crow::response execute(const communication::Command &command) const;
};
