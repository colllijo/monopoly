#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/Logger.hpp"

using communication::Command;
using communication::CommandResult;

using namespace communication::commands;

class GameService
{
public:
	GameService(const std::shared_ptr<CommunicationService>& communication);

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;

	CommandResult playerJoinRoom(const nlohmann::json& command) const;
	CommandResult startGame(const nlohmann::json& command) const;
	CommandResult makeMove(const nlohmann::json& command) const;
	// CommandResult transferMoney(const nlohmann::json& command) const;
};
