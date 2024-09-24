#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace communication
{
enum CommandQueue
{
	GAME,
	DATA,
};

struct Command
{
	std::string command;
	CommandQueue queue;
	std::optional<nlohmann::json> data;

	bool operator<(const Command& other) const {
            return command < other.command;
	}

	bool operator==(const Command& other) const {
		return command == other.command;
	}
};

inline void to_json(nlohmann::json& j, const Command& cmd) {
    j = nlohmann::json{
        {"command", cmd.command},
        {"queue", cmd.queue},
        {"data", cmd.data ? *cmd.data : nullptr},
    };
}

inline void from_json(const nlohmann::json& j, Command& cmd) {
    j.at("command").get_to(cmd.command);
    j.at("queue").get_to(cmd.queue);
    if (j.contains("data") && !j["data"].is_null()) {
        cmd.data = j["data"];
    } else {
        cmd.data.reset();
    }
}

namespace commands
{
	struct GetGames : public communication::Command {
		GetGames() : Command("GetGames", communication::CommandQueue::DATA, std::nullopt) {}
	};

	struct CreateGame : public communication::Command {
		CreateGame() : Command("CreateGame", communication::CommandQueue::DATA, std::nullopt) {}
		CreateGame(nlohmann::json data) : Command("CreateGame", communication::CommandQueue::DATA, data) {}
	};
}

};  // namespace communication
