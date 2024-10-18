#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

using nlohmann::json;

namespace communication
{
	enum CommandQueue
	{
		GAME,
		DATA,
		PUSH,
	};

	const std::unordered_map<CommandQueue, std::string_view> QUEUE_NAMES = {
	    {CommandQueue::GAME, "game_queue"},
	    {CommandQueue::DATA, "data_queue"},
	    {CommandQueue::PUSH, "push_queue"},
	};

	inline std::string_view getQueuenName(CommandQueue queue) { return QUEUE_NAMES.at(queue); }

	struct CommandData
	{
	};

	struct Command
	{
		std::string name;
		CommandQueue queue;
		CommandData data;

		Command() = default;
		Command(const std::string& name, const CommandQueue& queue) : name(name), queue(queue){};

		virtual json toJson() const { return {{"name", name}, {"queue", queue}}; }

		bool operator<(const Command& other) const { return name < other.name; }

		bool operator==(const Command& other) const { return name == other.name; }

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Command, name, queue);
	};

	typedef json CommandResult;

	namespace commands
	{

		// GetFieldByPosition
		struct BuyFieldData : public CommandData
		{
			std::string gameId;
			std::string playerId;
			std::string fieldId;

			BuyFieldData() = default;
			BuyFieldData(const std::string& gameId, const std::string& playerId, const std::string& fieldId) : gameId(gameId), playerId(playerId), fieldId(fieldId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(BuyFieldData, gameId, playerId, fieldId);
		};

		struct BuyField : public Command
		{
			BuyFieldData data;

			BuyField() : Command("BuyField", CommandQueue::DATA){};
			BuyField(const std::string& gameId, const std::string& playerId, const std::string& fieldId)
			    : Command("BuyField", CommandQueue::DATA), data(gameId, playerId, fieldId){};

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(BuyField, name, queue, data);
		};

		// GetFieldByPosition
		struct PlayerBuyFieldData : public CommandData
		{
			std::string roomId;
			std::string playerId;

			PlayerBuyFieldData() = default;
			PlayerBuyFieldData(const std::string& roomId, const std::string& playerId) : roomId(roomId), playerId(playerId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerBuyFieldData, roomId, playerId);
		};

		struct PlayerBuyField : public Command
		{
			PlayerBuyFieldData data;

			PlayerBuyField() : Command("PlayerBuyField", CommandQueue::GAME){};
			PlayerBuyField(const std::string& roomId, const std::string& playerId) : Command("PlayerBuyField", CommandQueue::GAME), data(roomId, playerId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerBuyField, name, queue, data);
		};

		// GetFieldByPosition
		struct GetFieldByPositionData : public CommandData
		{
			std::string gameId;
			int position;

			GetFieldByPositionData() = default;
			GetFieldByPositionData(const std::string& gameId, const int& position) : gameId(gameId), position(position) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetFieldByPositionData, gameId, position);
		};

		struct GetFieldByPosition : public Command
		{
			GetFieldByPositionData data;

			GetFieldByPosition() : Command("GetFieldByPosition", CommandQueue::DATA){};
			GetFieldByPosition(const std::string& gameId, const int& position) : Command("GetFieldByPosition", CommandQueue::DATA), data(gameId, position) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetFieldByPosition, name, queue, data);
		};

		// PlayerStartTurn
		struct PlayerStartTurnData : public CommandData
		{
			std::string roomId;
			std::string playerId;

			PlayerStartTurnData() = default;
			PlayerStartTurnData(const std::string& roomId, const std::string& playerId) : roomId(roomId), playerId(playerId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerStartTurnData, roomId, playerId);
		};

		struct PlayerStartTurn : public Command
		{
			PlayerStartTurnData data;

			PlayerStartTurn() : Command("PlayerStartTurn", CommandQueue::GAME) {}
			PlayerStartTurn(const std::string& roomId, const std::string& playerId) : Command("PlayerStartTurn", CommandQueue::GAME), data(roomId, playerId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerStartTurn, name, queue, data);
		};

		// PlayerEndTurn
		struct PlayerEndTurnData : public CommandData
		{
			std::string playerId;
			std::string roomId;

			PlayerEndTurnData() = default;
			PlayerEndTurnData(const std::string& playerId, const std::string& roomId) : playerId(playerId), roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerEndTurnData, playerId, roomId);
		};

		struct PlayerEndTurn : public Command
		{
			PlayerEndTurnData data;

			PlayerEndTurn() : Command("PlayerEndTurn", CommandQueue::GAME) {}
			PlayerEndTurn(const std::string& playerId, const std::string& roomId) : Command("PlayerEndTurn", CommandQueue::GAME), data(playerId, roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerEndTurn, name, queue, data);
		};

		// GetPlayerById
		struct GetPlayerByIdData : public CommandData
		{
			std::string playerId;

			GetPlayerByIdData() = default;
			GetPlayerByIdData(const std::string& playerId) : playerId(playerId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetPlayerByIdData, playerId);
		};

		struct GetPlayerById : public Command
		{
			GetPlayerByIdData data;

			GetPlayerById() : Command("GetPlayerById", CommandQueue::DATA) {}
			GetPlayerById(const std::string& playerId) : Command("GetPlayerById", CommandQueue::DATA), data(playerId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetPlayerById, name, queue, data);
		};

		// UpdatePlayer
		struct UpdatePlayerData : public CommandData
		{
			std::string id;
			int position;
			int money;

			UpdatePlayerData() = default;
			UpdatePlayerData(const std::string& id, int position, int money) : id(id), position(position), money(money) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdatePlayerData, id, position, money);
		};

		struct UpdatePlayer : public Command
		{
			UpdatePlayerData data;

			UpdatePlayer() : Command("UpdatePlayer", CommandQueue::DATA) {}
			UpdatePlayer(const std::string& id, int position, int money) : Command("UpdatePlayer", CommandQueue::DATA), data(id, position, money) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(UpdatePlayer, name, queue, data);
		};

		// StartGame
		struct StartGameData : public CommandData
		{
			std::string roomId;

			StartGameData() = default;
			StartGameData(const std::string& roomId) : roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGameData, roomId);
		};

		struct StartGame : public Command
		{
			StartGameData data;

			StartGame() : Command("StartGame", CommandQueue::DATA) {}
			StartGame(const std::string& roomId) : Command("StartGame", CommandQueue::DATA), data(roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(StartGame, name, queue, data);
		};

		// GetGameByRoomId
		struct GetGameByRoomIdData : public CommandData
		{
			std::string roomId;

			GetGameByRoomIdData() = default;
			GetGameByRoomIdData(const std::string& roomId) : roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetGameByRoomIdData, roomId);
		};

		struct GetGameByRoomId : public Command
		{
			GetGameByRoomIdData data;

			GetGameByRoomId() : Command("GetGameByRoomId", CommandQueue::DATA) {}
			GetGameByRoomId(const std::string& roomId) : Command("GetGameByRoomId", CommandQueue::DATA), data(roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetGameByRoomId, name, queue, data);
		};

		// GameUpdateTurn
		struct GameUpdateTurnData : public CommandData
		{
			std::string roomId;

			GameUpdateTurnData() = default;
			GameUpdateTurnData(const std::string& roomId) : roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameUpdateTurnData, roomId);
		};

		struct GameUpdateTurn : public Command
		{
			GameUpdateTurnData data;

			GameUpdateTurn() : Command("GameUpdateTurn", CommandQueue::DATA) {}
			GameUpdateTurn(const std::string& roomId) : Command("GameUpdateTurn", CommandQueue::DATA), data(roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameUpdateTurn, name, queue, data);
		};

		// GetPlayersByRoomId
		struct GetPlayersByRoomIdData : public CommandData
		{
			std::string roomId;

			GetPlayersByRoomIdData() = default;
			GetPlayersByRoomIdData(const std::string& roomId) : roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetPlayersByRoomIdData, roomId);
		};

		struct GetPlayersByRoomId : public Command
		{
			GetPlayersByRoomIdData data;

			GetPlayersByRoomId() : Command("GetPlayersByRoomId", CommandQueue::DATA) {}
			GetPlayersByRoomId(const std::string& roomId) : Command("GetPlayersByRoomId", CommandQueue::DATA), data(roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetPlayersByRoomId, name, queue, data);
		};

		// PlayerJoinRoom
		struct PlayerJoinRoomData : public CommandData
		{
			std::string username;
			std::string roomId;

			PlayerJoinRoomData() = default;
			PlayerJoinRoomData(const std::string& username, const std::string& roomId) : username(username), roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerJoinRoomData, username, roomId);
		};

		struct PlayerJoinRoom : public Command
		{
			PlayerJoinRoomData data;

			PlayerJoinRoom() : Command("PlayerJoinRoom", CommandQueue::GAME) {}
			PlayerJoinRoom(const std::string& username, const std::string& roomId) : Command("PlayerJoinRoom", CommandQueue::GAME), data(username, roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerJoinRoom, name, queue, data);
		};

		// Room Commands
		//	GetRooms
		struct GetRooms : public Command
		{
			GetRooms() : Command("GetRooms", CommandQueue::DATA) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetRooms, name, queue);
		};

		//	GetRoomByPlayerId
		struct GetRoomByPlayerIdData : public CommandData
		{
			std::string playerId;

			GetRoomByPlayerIdData() = default;
			GetRoomByPlayerIdData(const std::string& playerId) : playerId(playerId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetRoomByPlayerIdData, playerId);
		};

		struct GetRoomByPlayerId : public Command
		{
			GetRoomByPlayerIdData data;

			GetRoomByPlayerId() : Command("GetRoomByPlayerId", CommandQueue::DATA) {}
			GetRoomByPlayerId(const std::string& playerId) : Command("GetRoomByPlayerId", CommandQueue::DATA), data(playerId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(GetRoomByPlayerId, name, queue, data);
		};

		//	CreateRoom
		struct CreateRoomData : public CommandData
		{
			std::string roomName;
			std::string username;

			CreateRoomData() = default;
			CreateRoomData(const std::string& roomName, const std::string& username) : roomName(roomName), username(username) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(CreateRoomData, roomName, username);
		};

		struct CreateRoom : public Command
		{
			CreateRoomData data;

			CreateRoom() : Command("CreateRoom", communication::CommandQueue::DATA) {}
			CreateRoom(const std::string& roomName, const std::string& username) : Command("CreateRoom", communication::CommandQueue::DATA), data(roomName, username) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(CreateRoom, name, queue, data);
		};

		//	JoinRoom
		struct JoinRoomData : public CommandData
		{
			std::string username;
			std::string roomId;

			JoinRoomData() = default;
			JoinRoomData(const std::string& username, const std::string& roomId) : username(username), roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(JoinRoomData, username, roomId);
		};

		struct JoinRoom : public Command
		{
			JoinRoomData data;

			JoinRoom() : Command("JoinRoom", CommandQueue::DATA) {}
			JoinRoom(const std::string& username, const std::string& roomId) : Command("JoinRoom", CommandQueue::DATA), data(username, roomId) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(JoinRoom, name, queue, data);
		};

		//	LeaveRoom
		struct LeaveRoomData : public CommandData
		{
			std::string userId;
			std::string roomId;

			LeaveRoomData() = default;
			LeaveRoomData(const std::string& userId, const std::string& roomId) : userId(userId), roomId(roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(LeaveRoomData, userId, roomId);
		};

		struct LeaveRoom : public Command
		{
			LeaveRoomData data;

			LeaveRoom() : Command("LeaveRoom", CommandQueue::GAME) {}
			LeaveRoom(const std::string& userId, const std::string& roomId) : Command("LeaveRoom", CommandQueue::GAME), data(userId, roomId) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(LeaveRoom, name, queue, data);
		};

		// WebSocket Commands
		//	Push
		enum class PushTarget
		{
			ROOM,
			PLAYER,
		};

		struct PushData : public CommandData
		{
			std::string receiver;
			PushTarget target;
			json message;

			PushData() = default;
			PushData(const std::string& receiver, const PushTarget& target, const json& message) : receiver(receiver), target(target), message(message) {}

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(PushData, receiver, target, message);
		};

		struct Push : public Command
		{
			PushData data;

			Push() : Command("Push", CommandQueue::PUSH) {}
			Push(const std::string& receiver, const PushTarget& target, const json& message) : Command("Push", CommandQueue::PUSH), data(receiver, target, message) {}

			json toJson() const override { return {{"name", name}, {"queue", queue}, {"data", data}}; }

			NLOHMANN_DEFINE_TYPE_INTRUSIVE(Push, name, queue, data);
		};
	};  // namespace commands
};      // namespace communication
