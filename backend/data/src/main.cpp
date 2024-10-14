#include <memory>
#include <pqxx/pqxx>

#include "Database.hpp"
#include "GameRepository.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main()
{
	std::shared_ptr<SpdLogger> logger = std::make_shared<SpdLogger>();
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();
	std::shared_ptr<Database> database = std::make_shared<Database>();

	communicationService->setLogger(logger);
	database->setLogger(logger);

	database->initialize();
	if (!database->isDataInitialized()) return 1;

	// Declare handlers
	GameRepository gameRepository(communicationService, database);
	gameRepository.setLogger(logger);

	// Initialize DB schema
	if (!database->isDataInitialized()) return 1;

	logger->info("Starting the DataService");
	communicationService->start();

	return 0;
}
