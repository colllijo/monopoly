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
    : logger(NoOptLogger::getInstance()), communication(communication), dbConnection(dbConnection)
{
	communication->handleCommand(communication::commands::data::GetGames(), std::bind(&GameRepository::getGames, this, std::placeholders::_1));
	communication->handleCommand(communication::commands::data::CreateGame(), std::bind(&GameRepository::createGame, this, std::placeholders::_1));
}

CommandResult GameRepository::getGames(const Command&)
{
	pqxx::result result;

	try {
		pqxx::work txn(*dbConnection);

		result = txn.exec("SELECT * FROM games");

		txn.commit();
	} catch (const pqxx::sql_error &e) {
		logger->error("Failed to get games: {}", e.what());
		return CommandResult();
	}

	CommandResult data = nlohmann::json::array();
	for (const pqxx::row &row : result)
		data.push_back(game::Game(row).toJson());

	return data;
}

CommandResult GameRepository::createGame(const Command &command)
{
	if (
		!command.data.has_value()
		|| !command.data->contains("name")
	) return CommandResult();

	std::string name = command.data->value("name", "");
	std::string user = command.data->value("user", "");

	pqxx::result result;

	try {
		pqxx::work txn(*dbConnection);

		result = txn.exec(
			R"(
				INSERT INTO games (name) VALUES ($1)
				RETURNING *
			)",
			pqxx::params{name}
		);

		txn.commit();
		
		logger->info("Created game: {}", name);
	} catch (const pqxx::sql_error &e) {
		logger->error("Failed to create game: {}", e.what());
		return CommandResult();
	}

	return game::Game(result[0]).toJson();
}
