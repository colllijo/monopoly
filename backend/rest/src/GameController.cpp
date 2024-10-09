#include "GameController.hpp"

#include <nlohmann/json.hpp>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameController::GameController(const std::shared_ptr<crow::SimpleApp> &app, const std::shared_ptr<CommunicationService> &communication)
    : logger(NoOptLogger::getInstance()), app(app), communication(communication)
{
	registerRoutes();
}

void GameController::registerRoutes() const
{
	CROW_ROUTE((*app), "/games").methods(crow::HTTPMethod::GET)([this]() { return handleGetGames(); });

	CROW_ROUTE((*app), "/games").methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return handleCreateGame(req); });

	CROW_ROUTE((*app), "/games/<string>").methods(crow::HTTPMethod::POST)([this](const crow::request &req, std::string id) { return handleJoinGame(req, id); });

	CROW_ROUTE((*app), "/games/<string>").methods(crow::HTTPMethod::DELETE)([this](std::string id) { return handleLeaveGame(id); });
}

crow::response GameController::handleGetGames() const
{
	const communication::commands::GetGames command;

	return execute(command);
}

crow::response GameController::handleCreateGame(const crow::request &req) const
{
	nlohmann::json body;

	try
	{
		body = nlohmann::json::parse(req.body);
	}
	catch (const nlohmann::json::parse_error &e)
	{
		return crow::response(400);
	}

	if (!body.contains("name") || !body.contains("user")) return crow::response(400);

	const communication::commands::CreateGame command({{"name", body["name"]}, {"user", body["user"]}});

	return execute(command);
}

crow::response GameController::handleJoinGame(const crow::request &req, std::string &gameId) const
{
	nlohmann::json body;

	try
	{
		body = nlohmann::json::parse(req.body);
	}
	catch (const nlohmann::json::parse_error &e)
	{
		return crow::response(400);
	}

	if (!body.contains("name")) return crow::response(400);

	const communication::commands::JoinGame command({{"gameId", gameId}, {"name", body["name"]}});

	return execute(command);
}

crow::response GameController::handleLeaveGame(std::string &gameId) const
{
	if (gameId.empty()) return crow::response(400);

	const communication::commands::LeaveGame command(nlohmann::json({{"gameId", gameId}}));

	return execute(command);
}

crow::response GameController::execute(const communication::Command &command) const
{
	const nlohmann::json result = communication->execute(command);

	crow::response res(result.dump());
	res.add_header("Content-Type", "application/json");

	return res;
}
