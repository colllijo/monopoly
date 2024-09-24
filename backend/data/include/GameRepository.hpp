#pragma once

#include <memory>
#include <pqxx/pqxx>

#include "communication/CommunicationService.hpp"

class GameRepository
{
public:
	GameRepository(std::shared_ptr<CommunicationService> communication, std::shared_ptr<pqxx::connection> dbConnection);
private:
	std::shared_ptr<CommunicationService> communication;
	std::shared_ptr<pqxx::connection> dbConnection;

	nlohmann::json getGames(const communication::Command);
	nlohmann::json createGame(const communication::Command &command);
};
