#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace amqp
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
};  // namespace amqp
