#include <chrono>
#include <format>
#include <memory>
#include <pqxx/pqxx>
#include <string>
#include <thread>

#include "communication/Logger.hpp"
#include "communication/NoOptLogger.hpp"

#define MAX_CONNECTION_ATTEMPTS 4

static inline std::shared_ptr<pqxx::connection> createConnection(std::shared_ptr<Logger> logger = NoOptLogger::getInstance())
{
	const char *database = std::getenv("DATABASE");
	if (!database) database = "localhost";

	std::string connectionString = std::format("postgresql://pennybags:131044f9-0b69-4031-aad4-197901fcdcde@{}:5432/monopoly", database);

	auto backoffTime = std::chrono::seconds(1);
	auto prevBackoff = backoffTime;

	for (int attempt = 0; attempt < MAX_CONNECTION_ATTEMPTS; attempt++)
	{
		try
		{
			auto connection = std::make_shared<pqxx::connection>(connectionString);

			if (connection->is_open())
			{
				logger->info("Connected to database");
				return connection;
			}
		}
		catch (const pqxx::broken_connection &e)
		{
			if (attempt < MAX_CONNECTION_ATTEMPTS - 1)
			{
				logger->warn("Failed to connect to database, retrying in {} seconds", backoffTime.count());
				std::this_thread::sleep_for(backoffTime);

				backoffTime = backoffTime + prevBackoff;
				prevBackoff = backoffTime - prevBackoff;
			}
			else
			{
				logger->error("Failed to connect to database: {}", e.what());
			}
		}
	}

	return nullptr;
}

static inline bool createSchema(std::shared_ptr<pqxx::connection> connection, std::shared_ptr<Logger> logger = NoOptLogger::getInstance())
{
	try
	{
		pqxx::work txn(*connection);

		// GAME table
		txn.exec(R"(
			DO $$
			BEGIN
				IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'game_state') THEN
					CREATE TYPE game_state AS ENUM ('WAITING', 'RUNNING', 'FINISHED');
				END IF;
			END $$;
		)");
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS games (
				id SERIAL PRIMARY KEY,
				uuid UUID NOT NULL UNIQUE DEFAULT gen_random_uuid(),
				name VARCHAR(255) NOT NULL UNIQUE,
				state game_state NOT NULL DEFAULT 'WAITING',
				created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
			);
		)");

		// USER table
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS players (
				id SERIAL PRIMARY KEY,
				uuid UUID NOT NULL UNIQUE DEFAULT gen_random_uuid(),
				name VARCHAR(255) NOT NULL
			);
		)");

		// GAME_PLAYER table
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS game_players (
				id SERIAL PRIMARY KEY,
				game_id INT NOT NULL REFERENCES games(id),
				player_id INT NOT NULL REFERENCES players(id),
				hosting BOOLEAN NOT NULL DEFAULT FALSE
			);
		)");

		txn.commit();

		return true;
	}
	catch (const pqxx::sql_error &e)
	{
		logger->error("Failed to create schema: {}", e.what());

		return false;
	}
}
