#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

using communication::Command;
using communication::CommandResult;

class GameRepository
{
public:
	GameRepository(std::shared_ptr<CommunicationService> communication, std::shared_ptr<pqxx::connection> dbConnection);

	void setLogger(std::shared_ptr<Logger> logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;
	std::shared_ptr<pqxx::connection> dbConnection;

	CommandResult getGames(const Command& command);
	CommandResult createGame(const Command &command);
};
