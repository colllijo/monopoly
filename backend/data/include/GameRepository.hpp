#pragma once

#include <memory>
#include <pqxx/pqxx>

#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

class GameRepository
{
public:
	GameRepository(std::shared_ptr<CommunicationService> communication, std::shared_ptr<pqxx::connection> dbConnection);

	void setLogger(std::shared_ptr<Logger> logger) {
		this->logger = logger;
	}
private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;
	std::shared_ptr<pqxx::connection> dbConnection;

	nlohmann::json getGames(const communication::Command);
	nlohmann::json createGame(const communication::Command &command);
};
