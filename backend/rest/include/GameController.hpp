#pragma once

#include <crow.h>
#include <crow/http_request.h>

#include <memory>
#include <string>

#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

class GameController
{
public:
	GameController(const std::shared_ptr<crow::SimpleApp>& app, const std::shared_ptr<CommunicationService>& communication);

	void setLogger(std::shared_ptr<Logger> logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<crow::SimpleApp> app;
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
