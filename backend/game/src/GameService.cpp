#include "GameService.hpp"

#include "communication/NoOptLogger.hpp"

GameService::GameService(const std::shared_ptr<CommunicationService>& communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
	communication->registerCommandHandler(PlayerJoinRoom(), std::bind(&GameService::playerJoinRoom, this, std::placeholders::_1));
}
CommandResult GameService::playerJoinRoom(const nlohmann::json& command) const
{
	const PlayerJoinRoomData data = static_cast<PlayerJoinRoom>(command).data;

	const CommandResult result = communication->execute(std::make_shared<JoinRoom>(data.username, data.roomId));

	const auto push = std::make_shared<Push>();
	push->data.data = result;
	push->data.receiver = result["room"]["id"];

	communication->execute(push);

	return result;
}
