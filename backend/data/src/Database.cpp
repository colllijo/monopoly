#include "Database.hpp"

#include <thread>
#include <chrono>

#include "communication/NoOptLogger.hpp"

Database::Database(): logger(NoOptLogger::getInstance()), dataInitialized(false) {}

void Database::initialize()
{
	createConnection();
	createSchema();
	createProperties();
}

void Database::createConnection()
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
			connection = std::make_shared<pqxx::connection>(connectionString);

			if (connection->is_open())
			{
				logger->info("Connected to database");
				break;
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
}

void Database::createSchema()
{
	try
	{
		pqxx::work txn(*connection.get());

		// Room Table
		txn.exec(R"(
			DO $$
			BEGIN
				IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'room_state') THEN
					CREATE TYPE room_state AS ENUM ('LOBBY', 'STARTED', 'ENDED');
				END IF;
			END $$;
		)");
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS Room (
				id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
				name VARCHAR(255) NOT NULL,
				state room_state NOT NULL DEFAULT 'LOBBY'
			)
		)");

		// Player Table
		txn.exec(R"(
			DO $$
			BEGIN
				IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'player_color') THEN
					CREATE TYPE player_color AS ENUM ('RED', 'GREEN', 'BLUE', 'YELLOW');
				END IF;
			END $$;
		)");
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS Player (
				id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
				username VARCHAR(255) NOT NULL,
				room_id UUID REFERENCES Room(id),
				color player_color NOT NULL,
				money INT NOT NULL DEFAULT 30000,
				position INT NOT NULL DEFAULT 0
			)
		)");

		// Game Table
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS Game (
				id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
				room_id UUID REFERENCES Room(id),
				current_player_id UUID REFERENCES Player(id),
				round INT NOT NULL DEFAULT 0
			)
		)");

		// Field Table
		txn.exec(R"(
			DO $$
			BEGIN
				IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'field_type') THEN
					CREATE TYPE field_type AS ENUM ('PROPERTY', 'CHANCE', 'COMMUNITY_CHEST', 'TAX', 'GO', 'JAIL', 'FREE_PARKING', 'GO_TO_JAIL', 'RAILROAD', 'UTILITY');
				END IF;
			END $$;
		)");
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS Field (
				id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
				name VARCHAR(255) NOT NULL,
				type field_type NOT NULL,
				position INT NOT NULL,
				cost INT NOT NULL DEFAULT 0,
				rent INT NOT NULL DEFAULT 0
			)
		)");

		// Property Table
		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS Property (
				id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
				game_id UUID NOT NULL REFERENCES Game(id),
				player_id UUID NOT NULL REFERENCES Player(id),
				field_id UUID NOT NULL REFERENCES Field(id)
			)
		)");

		txn.commit();

		dataInitialized = true;
	}
	catch (const pqxx::sql_error &e) { logger->error("Failed to create schema: {}", e.what()); }
}

void Database::createProperties()
{
	try
	{
		pqxx::work txn(*connection.get());

		int fields = txn.query_value<int>("SELECT COUNT(*) FROM Field");
		if (fields != 0) return;

		txn.exec(R"(
			INSERT INTO Field (name, type, position, cost, rent) VALUES
				('Start', 'GO', 0, 0, 0),
				('Chur Kornplatz', 'PROPERTY', 1, 1200, 40),
				('Kanzlei', 'COMMUNITY_CHEST', 2, 0, 0),
				('Schaffhausen Vordergasse', 'PROPERTY', 3, 1200, 80),
				('Einkommenssteuer', 'TAX', 4, 0, 4000),
				('Vereinigte Privatbahnen', 'RAILROAD', 5, 2000, 200),
				('Aarau Rathausplatz', 'PROPERTY', 6, 2000, 120),
				('Chance', 'CHANCE', 7, 0, 0),
				('Neuenburg Place Purry', 'PROPERTY', 8, 2000, 120),
				('Thun Hauptgasse', 'PROPERTY', 9, 2400, 160),
				('BBZBL', 'JAIL', 10, 0, 0),
				('Basel Steinen-Vorstadt', 'PROPERTY', 11, 2800, 200),
				('Elektrizitätswerk', 'UTILITY', 12, 3000, 0),
				('Solothurn Hauptgasse', 'PROPERTY', 13, 2800, 200),
				('Lugano Via Nassa', 'PROPERTY', 14, 3200, 240),
				('Vereinigte Bergbahnen AG.', 'RAILROAD', 15, 4000, 0),
				('Biel Nidaugasse', 'PROPERTY', 16, 3600, 280),
				('Kanzlei', 'COMMUNITY_CHEST', 17, 0, 0),
				('Freiburg Bahnhofstrasse', 'PROPERTY', 18, 3600, 280),
				('La Chaux-de-Fonds Rue Lépo-Robert', 'PROPERTY', 19, 4000, 320),
				('Freier Parkplatz', 'FREE_PARKING', 20, 0, 0),
				('Winterthur Bahnofplatz', 'PROPERTY', 21, 4400, 360),
				('Chance', 'CHANCE', 22, 0, 0),
				('St. Gallen Marktplatz', 'PROPERTY', 23, 4400, 360),
				('Bern Bundesplatz', 'PROPERTY', 24, 4800, 400),
				('Überlandbahnen', 'RAILROAD', 25, 4000, 0),
				('Luzern Wggisgasse', 'PROPERTY', 26, 5200, 440),
				('Zürich Rennweg', 'PROPERTY', 27, 5200, 440),
				('Wasserwerke', 'UTILITY', 28, 3000, 0),
				('Lausanne Rue de Bourg', 'PROPERTY', 29, 5600, 440),
				('In die Schule', 'GO_TO_JAIL', 30, 0, 0),
				('Basel Freiestrasse', 'PROPERTY', 31, 6000, 520),
				('Genf Rue de la Croix-d''Or', 'PROPERTY', 32, 6000, 520),
				('Kanzlei', 'COMMUNITY_CHEST', 33, 0, 0),
				('Bern Spitalgasse', 'PROPERTY', 34, 6400, 560),
				('Vereinigte Schwebebahnen AG.', 'RAILROAD', 35, 4000, 0),
				('Chance', 'CHANCE', 36, 0, 0),
				('Lausanne Place St-François', 'PROPERTY', 37, 7000, 700),
				('Nachsteuer', 'TAX', 38, 0, 2000),
				('Zürich Paradeplatz', 'PROPERTY', 39, 8000, 1000)
		)");

		txn.commit();
	}
	catch (const pqxx::sql_error &e) { logger->error("Failed to create properties: {}", e.what()); }
}
