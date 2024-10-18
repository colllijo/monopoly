#include "GameRepository.hpp"

#include <functional>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameRepository::GameRepository(const std::shared_ptr<CommunicationService>& communication, const std::shared_ptr<Database>& database)
    : logger(NoOptLogger::getInstance()), communication(communication), database(database)
{
	communication->registerCommandHandler(StartGame(), std::bind(&GameRepository::startGame, this, std::placeholders::_1));
	communication->registerCommandHandler(GetGameByRoomId(), std::bind(&GameRepository::getGameByRoomId, this, std::placeholders::_1));
	communication->registerCommandHandler(GameUpdateTurn(), std::bind(&GameRepository::gameUpdateTurn, this, std::placeholders::_1));
	communication->registerCommandHandler(GetFieldByPosition(), std::bind(&GameRepository::getFieldByPosition, this, std::placeholders::_1));
	communication->registerCommandHandler(BuyField(), std::bind(&GameRepository::buyField, this, std::placeholders::_1));
}

CommandResult GameRepository::startGame(const nlohmann::json& command) const
{
	logger->info("StartGame command received");

	const StartGameData data = static_cast<StartGame>(command).data;

	auto txn = database->getTransaction();

	txn.exec(
	    R"(
			UPDATE Room
			SET state = 'STARTED'
			WHERE id = $1
		)",
	    pqxx::params{data.roomId});

	auto startingPlayer = txn.query_value<std::string>(
	    R"(
			SELECT id FROM Player
			WHERE room_id = $1 AND color = 'RED'
			LIMIT 1
		)",
	    pqxx::params{data.roomId});

	auto game = txn.exec(
	    R"(
			INSERT INTO Game (room_id, current_player_id)
			VALUES ($1, $2)
			RETURNING *
		)",
	    pqxx::params{data.roomId, startingPlayer});

	txn.commit();

	auto result = CommandResult{
	    {"id", game[0][0].as<std::string>()}, {"roomId", game[0][1].as<std::string>()}, {"currentPlayer", game[0][2].as<std::string>()}, {"round", game[0][3].as<int>()}};
	result["pushType"] = "StartGame";

	const auto push = std::make_shared<Push>();
	push->data.message = result;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = data.roomId;

	communication->executePush(push);

	return result;
}

CommandResult GameRepository::getGameByRoomId(const nlohmann::json& command) const
{
	logger->info("GetGameByRoomId command received");

	const GetGameByRoomIdData data = static_cast<GetGameByRoomId>(command).data;

	auto txn = database->getTransaction();

	auto game = txn.exec(
	    R"(
			SELECT * FROM Game
			WHERE room_id = $1
		)",
	    pqxx::params{data.roomId});

	txn.commit();

	return CommandResult{
	    {"id", game[0][0].as<std::string>()}, {"roomId", game[0][1].as<std::string>()}, {"currentPlayer", game[0][2].as<std::string>()}, {"round", game[0][3].as<int>()}};
}

CommandResult GameRepository::gameUpdateTurn(const nlohmann::json& command) const
{
	logger->info("GameUpdateTurn command received");

	const GameUpdateTurnData data = static_cast<GameUpdateTurn>(command).data;

	auto txn = database->getTransaction();

	auto gameQuery = txn.exec(
	    R"(
			SELECT * FROM Game
			WHERE room_id = $1
		)",
	    pqxx::params{data.roomId});

	txn.commit();

	nlohmann::json currentGame = {{"id", gameQuery[0][0].as<std::string>()},
	                              {"roomId", gameQuery[0][1].as<std::string>()},
	                              {"currentPlayer", gameQuery[0][2].as<std::string>()},
	                              {"round", gameQuery[0][3].as<int>()}};

	pqxx::result playersQuery = txn.exec(
	    R"(
			SELECT * FROM Player
			WHERE room_id = $1
		)",
	    pqxx::params{data.roomId});

	txn.commit();

	nlohmann::json players = nlohmann::json::array();

	for (const auto& player : playersQuery)
	{
		players.push_back({{"id", player[0].as<std::string>()},
		                   {"username", player[1].as<std::string>()},
		                   {"color", player[3].as<std::string>()},
		                   {"money", player[4].as<int>()},
		                   {"position", player[5].as<int>()}});
	}

	int round = currentGame["round"];
	auto currentPlayer = std::find_if(players.begin(), players.end(), [&](const nlohmann::json& player) { return player["id"] == currentGame["currentPlayer"]; }).value();

	nlohmann::json nextPlayer;
	if (currentPlayer["color"] == "RED")
	{
		nextPlayer = std::find_if(players.begin(), players.end(), [&](const nlohmann::json& player) { return player["color"] == "GREEN"; }).value();
	}
	else if (currentPlayer["color"] == "GREEN")
	{
		nextPlayer = std::find_if(players.begin(), players.end(), [&](const nlohmann::json& player) { return player["color"] == "BLUE"; }).value();
	}
	else if (currentPlayer["color"] == "BLUE")
	{
		nextPlayer = std::find_if(players.begin(), players.end(), [&](const nlohmann::json& player) { return player["color"] == "YELLOW"; }).value();
	}
	else
	{
		nextPlayer = std::find_if(players.begin(), players.end(), [&](const nlohmann::json& player) { return player["color"] == "RED"; }).value();
		round++;
	}

	auto game = txn.exec(
	    R"(
			UPDATE Game
			SET current_player_id = $1,
			round = $2
			WHERE room_id = $3
			RETURNING *
		)",
	    pqxx::params{nextPlayer["id"].get<std::string>(), round, data.roomId});

	txn.commit();

	return CommandResult{
	    {"id", game[0][0].as<std::string>()}, {"roomId", game[0][1].as<std::string>()}, {"currentPlayer", game[0][2].as<std::string>()}, {"round", game[0][3].as<int>()}};
}

CommandResult GameRepository::getFieldByPosition(const nlohmann::json& command) const
{
	logger->info("GetFieldByPosition command received");

	const GetFieldByPositionData data = static_cast<GetFieldByPosition>(command).data;

	auto txn = database->getTransaction();

	const pqxx::result field = txn.exec(
	    R"(
			SELECT * FROM Field
			WHERE position = $1
		)",
	    pqxx::params{data.position});

	const auto owner = txn.exec(
	    R"(
			SELECT player_id FROM Property
			WHERE game_id = $1 AND field_id = $2;
		)",
	    pqxx::params{data.gameId, field[0]["id"].as<std::string>()});
	std::string ownerId;
	if (!owner.empty()) ownerId = owner[0]["player_id"].as<std::string>();

	const auto& fieldObj = field[0];

	return CommandResult{{"id", fieldObj["id"].as<std::string>()},
	                     {"name", fieldObj["name"].as<std::string>()},
	                     {"type", fieldObj["type"].as<std::string>()},
	                     {"position", fieldObj["position"].as<int>()},
	                     {"cost", fieldObj["cost"].as<int>()},
	                     {"rent", fieldObj["rent"].as<int>()},
	                     {"owner", ownerId}};
}

CommandResult GameRepository::buyField(const nlohmann::json& command) const
{
	logger->info("BuyField command received");

	const BuyFieldData data = static_cast<BuyField>(command).data;

	auto txn = database->getTransaction();

	txn.exec(
	    R"(
			INSERT INTO Property (game_id, player_id, field_id)
			VALUES ($1, $2, $3)
		)",
	    pqxx::params(data.gameId, data.playerId, data.fieldId));

	txn.commit();

	const pqxx::result field = txn.exec(
	    R"(
			SELECT * FROM Field
			WHERE id = $1
		)",
	    pqxx::params{data.fieldId});

	const auto& fieldObj = field[0];

	return CommandResult{{"name", fieldObj["name"].as<std::string>()}, {"type", fieldObj["type"].as<std::string>()}, {"position", fieldObj["position"].as<int>()},
	                     {"cost", fieldObj["cost"].as<int>()},         {"rent", fieldObj["rent"].as<int>()},         {"owner", data.playerId}};
}
