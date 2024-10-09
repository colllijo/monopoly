#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

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

	struct Command
	{
		std::string command;
		CommandQueue queue;
		std::optional<nlohmann::json> data;

		bool operator<(const Command& other) const { return command < other.command; }

		bool operator==(const Command& other) const { return command == other.command; }
	};

	inline void to_json(nlohmann::json& j, const Command& cmd)
	{
		j = nlohmann::json{
		    {"command", cmd.command},
		    {"queue", cmd.queue},
		    {"data", cmd.data ? *cmd.data : nullptr},
		};
	}

	inline void from_json(const nlohmann::json& j, Command& cmd)
	{
		j.at("command").get_to(cmd.command);
		j.at("queue").get_to(cmd.queue);
		if (j.contains("data") && !j["data"].is_null())
			cmd.data = j["data"];
		else
			cmd.data.reset();
	}

	typedef nlohmann::json CommandResult;

	namespace commands
	{
		struct GetGames : public communication::Command
		{
			GetGames() : Command("GetGames", communication::CommandQueue::GAME, std::nullopt) {}
		};

		struct CreateGame : public communication::Command
		{
			CreateGame() : Command("CreateGame", communication::CommandQueue::GAME, std::nullopt) {}
			CreateGame(nlohmann::json data) : Command("CreateGame", communication::CommandQueue::GAME, data) {}
		};

		struct JoinGame : public communication::Command
		{
			JoinGame() : Command("JoinGame", communication::CommandQueue::GAME, std::nullopt) {}
			JoinGame(nlohmann::json data) : Command("JoinGame", communication::CommandQueue::GAME, data) {}
		};

		struct LeaveGame : public communication::Command
		{
			LeaveGame() : Command("LeaveGame", communication::CommandQueue::GAME, std::nullopt) {}
			LeaveGame(nlohmann::json data) : Command("LeaveGame", communication::CommandQueue::GAME, data) {}
		};

		namespace data
		{
			struct GetGames : public communication::Command
			{
				GetGames() : Command("DataGetGames", communication::CommandQueue::DATA, std::nullopt) {}
			};

			struct CreateGame : public communication::Command
			{
				CreateGame() : Command("DataCreateGame", communication::CommandQueue::DATA, std::nullopt) {}
				CreateGame(nlohmann::json data) : Command("DataCreateGame", communication::CommandQueue::DATA, data) {}
			};
		};  // namespace data
	};  // namespace commands
};  // namespace communication
