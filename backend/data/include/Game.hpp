#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>

namespace game
{
	struct Game
	{
		std::string uuid;
		std::string name;
		std::string state;
		int players;

		Game(const pqxx::row &row)
		{
			if (!row["uuid"].is_null()) uuid = row["uuid"].as<std::string>();
			if (!row["name"].is_null()) name = row["name"].as<std::string>();
			if (!row["state"].is_null()) state = row["state"].as<std::string>();
			if (!row["players"].is_null()) players = row["players"].as<int>();
		}

		nlohmann::json toJson() const { return {{"uuid", uuid}, {"name", name}, {"state", state}, {"players", players}}; }
	};
};  // namespace game
