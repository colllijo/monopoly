#include <format>
#include <memory>
#include <pqxx/pqxx>
#include "communication/NoOptLogger.hpp"

static inline std::shared_ptr<pqxx::connection> createConnection() {
    const char* database = std::getenv("DATABASE");
    if (!database) database = "localhost";
	
	return std::make_shared<pqxx::connection>(std::format("postgresql://pennybags:131044f9-0b69-4031-aad4-197901fcdcde@{}:5432/monopoly", database));
}

static inline bool createSchema(std::shared_ptr<pqxx::connection> connection, std::shared_ptr<Logger> logger = std::make_shared<NoOptLogger>())
{
	try {
		pqxx::work txn(*connection);

		txn.exec(R"(
			CREATE TABLE IF NOT EXISTS games (
				id SERIAL PRIMARY KEY,
				name VARCHAR(255) NOT NULL UNIQUE,
				created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
			);
		)");

		txn.commit();

		return true;
	} catch (const pqxx::sql_error &e) {
		logger->error("Failed to create schema: {}", e.what());

		return false;
	}
}
