#include "GameService.hpp"

#include "communication/NoOptLogger.hpp"

GameService::GameService(const std::shared_ptr<CommunicationService>& communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
	communication->registerCommandHandler(PlayerJoinRoom(), std::bind(&GameService::playerJoinRoom, this, std::placeholders::_1));
}
CommandResult GameService::playerJoinRoom(const nlohmann::json& command) const
{
	logger->info("PlayerJoinRoom command received");

	const PlayerJoinRoomData data = static_cast<PlayerJoinRoom>(command).data;

	const CommandResult result = communication->execute(std::make_shared<JoinRoom>(data.username, data.roomId));
	logger->info("Player added to room");

	const auto push = std::make_shared<Push>();
	push->data.message = result;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = result["room"]["id"].get<std::string>();

	communication->executePush(push);

	return result;
}
