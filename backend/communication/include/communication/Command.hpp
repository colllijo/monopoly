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

inline void to_json(nlohmann::json& j, const struct Command& c)
{
	j = nlohmann::json{
	    {"command", c.command},
	    {"queue", c.queue},
	};
}

inline void from_json(const nlohmann::json& j, struct Command& c)
{
	j.at("command").get_to(c.command);
	j.at("queue").get_to(c.queue);
}

namespace commands
{
	struct GetGames : public communication::Command {
		GetGames() : Command("GetGames", communication::CommandQueue::DATA, std::nullopt) {}
	};
}

};  // namespace communication
