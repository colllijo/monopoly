#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

namespace game
{
struct Game
{
	int id;
	std::string name;

	Game(const pqxx::row &row)
	{
		id = row["id"].as<int>();
		name = row["name"].as<std::string>();
	}

	nlohmann::json toJson() const
	{
		return {
			{"id", id},
			{"name", name}
		};
	}
};
};  // namespace game
