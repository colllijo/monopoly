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
		//	PlayerJoinRoom
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
