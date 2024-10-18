#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "Database.hpp"
#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"

using communication::Command;
using communication::CommandResult;

using namespace communication::commands;

class RoomRepository {
public:
	RoomRepository(const std::shared_ptr<CommunicationService> &communication, const std::shared_ptr<Database> &database);

	void setLogger(const std::shared_ptr<Logger>& logger) { this->logger = logger; }

private:
	std::shared_ptr<Logger> logger;

	std::shared_ptr<CommunicationService> communication;
	std::shared_ptr<Database> database;

	CommandResult getRooms(const nlohmann::json&) const;
	CommandResult getRoomByPlayerId(const nlohmann::json&) const;
	CommandResult getPlayersByRoomId(const nlohmann::json&) const;
	CommandResult getPlayerById(const nlohmann::json&) const;
	CommandResult updatePlayer(const nlohmann::json&) const;
	CommandResult createRoom(const nlohmann::json& command) const;
	CommandResult joinRoom(const nlohmann::json& command) const;
};
