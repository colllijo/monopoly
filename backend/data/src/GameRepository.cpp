#include "GameRepository.hpp"

#include <functional>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "Game.hpp"
#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/NoOptLogger.hpp"

GameRepository::GameRepository(std::shared_ptr<CommunicationService> communication, std::shared_ptr<pqxx::connection> dbConnection)
    : logger(std::make_shared<NoOptLogger>()), communication(communication), dbConnection(dbConnection)
{
	communication->handleCommand(communication::commands::GetGames(), std::bind(&GameRepository::getGames, this, std::placeholders::_1));
	communication->handleCommand(communication::commands::CreateGame(), std::bind(&GameRepository::createGame, this, std::placeholders::_1));
}

nlohmann::json GameRepository::getGames(const communication::Command)
{
	pqxx::result result;

	try {
		pqxx::work txn(*dbConnection);

		result = txn.exec("SELECT * FROM games");

		txn.commit();
	} catch (const pqxx::sql_error &e) {
		return nlohmann::json();
	}

	nlohmann::json data = nlohmann::json::array();
	for (const pqxx::row &row : result)
		data.push_back(game::Game(row).toJson());

	return data;
}

nlohmann::json GameRepository::createGame(const communication::Command &command)
{
	if (!command.data.has_value() || !command.data->contains("name"))
		return nlohmann::json();

	std::string name = command.data->value("name", "");
	pqxx::result result;

	try {
		pqxx::work txn(*dbConnection);

		txn.exec(
			R"(INSERT INTO games (name) VALUES ($1))",
			pqxx::params{name}
		);

		result = txn.exec(
			R"(SELECT * FROM games WHERE name = $1 LIMIT 1)",
			pqxx::params{name}
		);

		txn.commit();
	} catch (const pqxx::sql_error &e) {
		logger->error("Failed to create game: {}", e.what());
		return nlohmann::json();
	}

	return game::Game(result[0]).toJson();
}
