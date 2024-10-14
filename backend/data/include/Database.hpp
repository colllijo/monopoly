#pragma once

#include <memory>
#include <pqxx/pqxx>

#include "communication/Logger.hpp"

#define MAX_CONNECTION_ATTEMPTS 4

class Database
{
public:
	Database();
	~Database() = default;

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

	void initialize();
	bool isDataInitialized() const { return dataInitialized; }

	pqxx::work getTransaction() const { return pqxx::work(*connection.get()); }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<pqxx::connection> connection;

	bool dataInitialized;

	void createConnection();
	void createSchema();
	void createProperties();
};
