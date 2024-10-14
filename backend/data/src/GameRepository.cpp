#include "GameRepository.hpp"
#include <functional>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameRepository::GameRepository(const std::shared_ptr<CommunicationService> &communication, const std::shared_ptr<Database> &database)
	: logger(NoOptLogger::getInstance()), communication(communication), database(database)
{
	communication->registerCommandHandler(GetRooms(), std::bind(&GameRepository::getRooms, this, std::placeholders::_1));
	communication->registerCommandHandler(CreateRoom(), std::bind(&GameRepository::createRoom, this, std::placeholders::_1));
	communication->registerCommandHandler(JoinRoom(), std::bind(&GameRepository::joinRoom, this, std::placeholders::_1));
}

CommandResult GameRepository::getRooms(const nlohmann::json &) const
{
	auto txn = database->getTransaction();

	pqxx::result rooms = txn.exec(
		R"(
			SELECT Room.id, Room.name, Room.state, COUNT(Player.id) as players FROM Room
			LEFT JOIN Player ON Room.id = Player.room_id
			GROUP BY Room.id, Room.name, Room.state
		)"
	);

	nlohmann::json result = nlohmann::json::array();

	for (const auto &room : rooms)
	{
		result.push_back({
			{"id", room[0].as<std::string>()},
			{"name", room[1].as<std::string>()},
			{"state", room[2].as<std::string>()},
			{"players", room[3].as<int>()}
		});
	}

	return result;
}

CommandResult GameRepository::createRoom(const nlohmann::json &command) const
{
	CreateRoomData data = static_cast<CreateRoom>(command).data;

	auto txn = database->getTransaction();

	pqxx::result room = txn.exec(
		R"(
			INSERT INTO Room (name)
			VALUES ($1)
			RETURNING *
		)",
		pqxx::params{data.roomName}
	);

	pqxx::result player = txn.exec(
		R"(
			INSERT INTO Player (username, room_id, color)
			VALUES ($1, $2, 'RED')
			RETURNING *
		)",
		pqxx::params{data.username, room[0][0].as<std::string>()}
	);

	txn.commit();

	return CommandResult{
		{"player", {
			{"id", player[0][0].as<std::string>()},
			{"username", player[0][1].as<std::string>()},
			{"color", player[0][3].as<std::string>()},
			{"money", player[0][4].as<int>()},
			{"position", player[0][5].as<int>()}
		}},
		{"room", {
			{"id", room[0][0].as<std::string>()},
			{"name", room[0][1].as<std::string>()},
			{"state", room[0][2].as<std::string>()}
		}}
	};
}

CommandResult GameRepository::joinRoom(const nlohmann::json &command) const
{
	logger->info("Joining room: {}", command.dump());
	JoinRoomData data = static_cast<JoinRoom>(command).data;

	auto txn = database->getTransaction();

	pqxx::result room = txn.exec(
		R"(
			SELECT * FROM Room
			WHERE id = $1
		)",
		pqxx::params{data.roomId}
	);

	if (room.empty())
	{
		return CommandResult{
			{"error", "Room not found"}
		};
	}

	pqxx::result player = txn.exec(
		R"(
			INSERT INTO Player (username, room_id, color)
			VALUES ($1, $2, 'GREEN')
			RETURNING *
		)",
		pqxx::params{data.username, data.roomId}
	);

	txn.commit();

	return CommandResult{
		{"player", {
			{"id", player[0][0].as<std::string>()},
			{"username", player[0][1].as<std::string>()},
			{"color", player[0][3].as<std::string>()},
			{"money", player[0][4].as<int>()},
			{"position", player[0][5].as<int>()}
		}},
		{"room", {
			{"id", room[0][0].as<std::string>()},
			{"name", room[0][1].as<std::string>()},
			{"state", room[0][2].as<std::string>()}
		}}
	};
}
