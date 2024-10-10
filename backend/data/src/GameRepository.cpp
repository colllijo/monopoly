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

		result = txn.exec(R"(
			SELECT 
				games.uuid, 
				games.name, 
				games.state, 
				COUNT(game_players.player_id) AS players
			FROM 
				games
			LEFT JOIN 
				game_players ON games.id = game_players.game_id
			GROUP BY 
				games.uuid, games.name, games.state
		)");

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

	try {
		pqxx::work txn(*dbConnection);

		auto player = txn.exec(
			R"(
				INSERT INTO players (name) VALUES ($1)
				RETURNING *
			)",
			pqxx::params{user}
		);

		auto game = txn.exec(
			R"(
				INSERT INTO games (name) VALUES ($1)
				RETURNING games.id, games.uuid, games.name, games.state, 1 AS players
			)",
			pqxx::params{name}
		);

		txn.exec(
			R"(
				INSERT INTO game_players (game_id, player_id, hosting) VALUES ($1, $2, TRUE)
			)",
			pqxx::params{game[0]["id"].as<int>(), player[0]["id"].as<int>()}
		);

		txn.commit();
		
		logger->info("Created game: {}", name);
		return game::Game(game[0]).toJson();
	} catch (const pqxx::sql_error &e) {
		logger->error("Failed to create game: {}", e.what());
	}

	return CommandResult();
}
