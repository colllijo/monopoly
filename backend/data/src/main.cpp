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
	std::shared_ptr<pqxx::connection> dbConnection = createConnection(logger);

	communicationService->setLogger(logger);

	if (!dbConnection) return 1;

	// Declare handlers
	GameRepository gameRepository(communicationService, dbConnection);
	gameRepository.setLogger(logger);

	// Initialize DB schema
	if (!createSchema(dbConnection, logger)) return 1;

	logger->info("Starting the DataService");
	communicationService->start();

	return 0;
}
