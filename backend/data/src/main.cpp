#include <memory>
#include <pqxx/pqxx>

#include "Database.hpp"
#include "RoomRepository.hpp"
#include "GameRepository.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main()
{
	const auto logger = std::make_shared<SpdLogger>();
	const auto communicationService = std::make_shared<CommunicationService>();
	const auto database = std::make_shared<Database>();

	communicationService->setLogger(logger);
	database->setLogger(logger);

	database->initialize();
	if (!database->isDataInitialized()) return 1;

	// Declare handlers
	RoomRepository roomRepository(communicationService, database);
	GameRepository gameRepository(communicationService, database);
	roomRepository.setLogger(logger);
	gameRepository.setLogger(logger);

	// Initialize DB schema
	if (!database->isDataInitialized()) return 1;

	logger->info("Starting the DataService");
	communicationService->start();

	return 0;
}
