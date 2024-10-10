#include "GameService.hpp"

#include "communication/Command.hpp"
#include "communication/NoOptLogger.hpp"

GameService::GameService(std::shared_ptr<CommunicationService> communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
	communication->registerCommandHandler(communication::commands::GetGames(), std::bind(&GameService::getGames, this, std::placeholders::_1));
	communication->registerCommandHandler(communication::commands::CreateGame(), std::bind(&GameService::createGame, this, std::placeholders::_1));
}

CommandResult GameService::getGames(const communication::Command&)
{
	communication::commands::data::GetGames dataCommand;

	return communication->execute(dataCommand);

}

CommandResult GameService::createGame(const Command& command)
{
	communication::commands::data::CreateGame dataCommand(command.data.value());

	return communication->execute(dataCommand);
}
