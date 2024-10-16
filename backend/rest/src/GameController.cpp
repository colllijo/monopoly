#include "GameController.hpp"

#include <crow/app.h>

#include <nlohmann/json.hpp>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameController::GameController(const std::shared_ptr<crow::App<crow::CORSHandler>> &app, const std::shared_ptr<CommunicationService> &communication)
    : logger(NoOptLogger::getInstance()), app(app), communication(communication)
{
	registerRoutes();
}

void GameController::registerRoutes() const
{
	CROW_ROUTE((*app), "/games").methods(crow::HTTPMethod::GET)([this]() { return handleGetGames(); });

	CROW_ROUTE((*app), "/games").methods(crow::HTTPMethod::POST)([this](const crow::request &req) { return handleCreateRoom(req); });

	CROW_ROUTE((*app), "/games/<string>").methods(crow::HTTPMethod::POST)([this](const crow::request &req, const std::string &id) { return handleJoinGame(req, id); });
}

crow::response GameController::handleGetGames() const
{
	const std::shared_ptr<Command> command = std::make_shared<GetRooms>();

	return execute(command);
}

crow::response GameController::handleCreateRoom(const crow::request &req) const
{
	nlohmann::json body;

	try { body = nlohmann::json::parse(req.body); }
	catch (const nlohmann::json::parse_error &e) { return crow::response(400); }

	if (!body.contains("name") || !body.contains("user")) return crow::response(400);

	const std::shared_ptr<Command> command = std::make_shared<CreateRoom>(body["name"], body["user"]);

	return execute(command);
}

crow::response GameController::handleJoinGame(const crow::request &req, const std::string &roomId) const
{
	nlohmann::json body;

	try { body = nlohmann::json::parse(req.body); }
	catch (const nlohmann::json::parse_error &e) { return crow::response(400); }

	if (!body.contains("user")) return crow::response(400);

	const std::shared_ptr<Command> command = std::make_shared<PlayerJoinRoom>(body["user"], roomId);

	return execute(command);
}

crow::response GameController::execute(const std::shared_ptr<Command> &command) const
{
	const nlohmann::json result = communication->execute(command);

	crow::response res(result.dump());
	res.add_header("Content-Type", "application/json");

	return res;
}
