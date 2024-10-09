#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

namespace game
{
struct Game
{
	std::string uuid;
	std::string name;
	std::string state;

	Game(const pqxx::row &row)
	{
		uuid = row["uuid"].as<std::string>();
		name = row["name"].as<std::string>();
		state = row["state"].as<std::string>();
	}

	nlohmann::json toJson() const
	{
		return {
			{"uuid", uuid},
			{"name", name},
			{"state", state}
		};
	}
};
};  // namespace game
