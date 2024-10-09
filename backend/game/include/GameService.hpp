#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

using communication::Command;
using communication::CommandResult;

class GameService
{
public:
	GameService(std::shared_ptr<CommunicationService> communication);

	void setLogger(std::shared_ptr<Logger> logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;

	CommandResult getGames(const Command& command);
	CommandResult createGame(const Command& command);
};
