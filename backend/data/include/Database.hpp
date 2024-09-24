#include <iostream>
#include <memory>
#include <pqxx/pqxx>

static inline void createSchema(std::shared_ptr<pqxx::connection> connection)
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
	} catch (const pqxx::sql_error &e) {
		std::cerr << "Failed to create schema: " << e.what() << std::endl;
		throw;
	}
}
