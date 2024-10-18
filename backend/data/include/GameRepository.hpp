#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "Database.hpp"
#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"

using communication::Command;
using communication::CommandResult;

using namespace communication::commands;

class GameRepository {
public:
	GameRepository(const std::shared_ptr<CommunicationService> &communication, const std::shared_ptr<Database> &database);

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;
	std::shared_ptr<Database> database;

	CommandResult startGame(const nlohmann::json&) const;
	CommandResult getGameByRoomId(const nlohmann::json&) const;
	CommandResult gameUpdateTurn(const nlohmann::json&) const;
};
