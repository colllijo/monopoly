#include "GameService.hpp"

#include <random>

#include "communication/NoOptLogger.hpp"

GameService::GameService(const std::shared_ptr<CommunicationService>& communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
	communication->registerCommandHandler(PlayerJoinRoom(), std::bind(&GameService::playerJoinRoom, this, std::placeholders::_1));
	communication->registerCommandHandler(StartGame(), std::bind(&GameService::startGame, this, std::placeholders::_1));
}

CommandResult GameService::playerJoinRoom(const nlohmann::json& command) const
{
	logger->info("PlayerJoinRoom command received");

	const PlayerJoinRoomData data = static_cast<PlayerJoinRoom>(command).data;

	const CommandResult result = communication->execute(std::make_shared<JoinRoom>(data.username, data.roomId));
	logger->info("Player added to room");

	nlohmann::json message = result;
	message["pushType"] = "JoinRoom";

	const auto push = std::make_shared<Push>();
	push->data.message = message;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = result["room"]["id"].get<std::string>();

	communication->executePush(push);

	return result;
}

CommandResult GameService::startGame(const nlohmann::json& command) const
{
	logger->info("StartGame command received");

	const StartGameData data = static_cast<StartGame>(command).data;

	// todo @colllijo return updated gameobject
	const CommandResult result = communication->execute(std::make_shared<StartGame>(data.roomId));
	logger->info("Game started");

	const auto push = std::make_shared<Push>();
	push->data.message = result;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = data.roomId;

	communication->execute(push);

	return result;
}
CommandResult GameService::makeMove(const nlohmann::json& command) const
{
	logger->info("MakeMove command received");

	const MakeMoveGetNextPlayerData data = static_cast<MakeMoveGetNextPlayer>(command).data;

	// todo @colllijo return player on turn
	CommandResult player = communication->execute(std::make_shared<MakeMoveGetNextPlayer>(data.roomId));

	int playerCurrentField = player["position"];

	// Use random_device to generate a true random number
	std::random_device rd;
	// Define the range (1 to 6)
	std::uniform_int_distribution<int> dist(1, 6);
	// Generate and print a random number between 1 and 6
	int diceResult = dist(rd);
	int newPlayerPosition = (playerCurrentField + diceResult) % 40;

	player["position"] = newPlayerPosition;

	const auto push = std::make_shared<Push>();
	push->data.message = player;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = data.roomId;

	// todo @collijo check if playerId in prperties table return {"bought": true} if it is
	return communication->execute(std::make_shared<MakeMoveGetField>(player["id"], newPlayerPosition));
}
/*CommandResult GameService::transferMoney(const nlohmann::json& command) const
{
	logger->info("TransferMoney command received");

	const TransferMoneyData data = static_cast<TransferMoney>(command).data;
}*/