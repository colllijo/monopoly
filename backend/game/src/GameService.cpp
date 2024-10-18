#include "GameService.hpp"

#include <memory>
#include <random>

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameService::GameService(const std::shared_ptr<CommunicationService>& communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
	communication->registerCommandHandler(PlayerJoinRoom(), std::bind(&GameService::playerJoinRoom, this, std::placeholders::_1));
	communication->registerCommandHandler(PlayerStartTurn(), std::bind(&GameService::playerStartTurn, this, std::placeholders::_1));
	communication->registerCommandHandler(PlayerEndTurn(), std::bind(&GameService::playerEndTurn, this, std::placeholders::_1));
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

CommandResult GameService::playerStartTurn(const nlohmann::json& command) const
{
	logger->info("PlayerStartTurn command received");

	const PlayerStartTurnData data = static_cast<PlayerStartTurn>(command).data;

	CommandResult game = communication->execute(std::make_shared<GetGameByRoomId>(data.roomId));

	if (game["currentPlayer"] != data.playerId) return CommandResult();

	CommandResult player = communication->execute(std::make_shared<GetPlayerById>(data.playerId));
	int position = player["position"];

	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 6);

	int diceResult = dist(rd) + dist(rd);

	if (position + diceResult > 39) player["money"] += 2000;
	player["position"] = (position + diceResult) % 40;

	communication->execute(std::make_shared<UpdatePlayer>(data.playerId, player["position"], player["money"]));

	CommandResult players = communication->execute(std::make_shared<GetPlayersByRoomId>(data.roomId));
	players["pushType"] = "Players";

	const auto push = std::make_shared<Push>();
	push->data.message = players;
	push->data.target = PushTarget::ROOM;
	push->data.receiver = data.roomId;

	communication->executePush(push);

	players.erase("pushType");
	return CommandResult{
		{"field", "Wünsch dir was"},
	};
}

CommandResult GameService::playerEndTurn(const nlohmann::json& command) const
{
	logger->info("PlayerEndTurn command received");

	const PlayerEndTurnData data = static_cast<PlayerEndTurn>(command).data;

	CommandResult game = communication->execute(std::make_shared<GetGameByRoomId>(data.roomId));

	if (game["currentPlayer"] != data.playerId) return CommandResult();

	game = communication->execute(std::make_shared<GameUpdateTurn>(data.roomId));
	game["pushType"] = "NextTurn";
	
	const auto push = std::make_shared<Push>();
	push->data.message = game;
	push->data.target = PushTarget::PLAYER;
	push->data.receiver = game["currentPlayer"].get<std::string>();

	communication->executePush(push);

	game.erase("pushType");
	return game;
}
