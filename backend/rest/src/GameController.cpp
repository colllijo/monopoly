#include "GameController.hpp"

#include <crow/common.h>
#include <crow/http_request.h>

#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include "crow/http_response.h"
#include "nlohmann/json_fwd.hpp"

GameController::GameController(
	std::shared_ptr<crow::SimpleApp> app,
	std::shared_ptr<CommunicationService> communication
) : app(app), communication(communication)
{
	registerRoutes();
}

void GameController::registerRoutes()
{
	CROW_ROUTE((*app), "/games").methods(crow::HTTPMethod::GET)([this]() { return handleGetGames(); });

	CROW_ROUTE((*app), "/games")
	    .methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return handleCreateGame(req); });

	CROW_ROUTE((*app), "/games/<string>")
	    .methods(crow::HTTPMethod::POST)([this](std::string id) { return handleJoinGame(id); });

	CROW_ROUTE((*app), "/games/<string>")
	    .methods(crow::HTTPMethod::DELETE)([this](std::string id) { return handleLeaveGame(id); });
}

crow::response GameController::handleGetGames()
{
	communication::commands::GetGames command;

	nlohmann::json result = communication->execute(command);

	crow::response res(result.dump());
	res.add_header("Content-Type", "application/json");

	return res;
}

crow::response GameController::handleCreateGame(const crow::request &req) { return crow::response(200); }

crow::response GameController::handleJoinGame(std::string gameId) { return crow::response(200); }

crow::response GameController::handleLeaveGame(std::string gameId) { return crow::response(200); }
