#include <memory>
#include <pqxx/pqxx>

#include "GameRepository.hpp"
#include "Database.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main() {
	std::shared_ptr<SpdLogger> logger = std::make_shared<SpdLogger>();
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();
	std::shared_ptr<pqxx::connection> dbConnection = std::make_shared<pqxx::connection>("postgresql://pennybags:131044f9-0b69-4031-aad4-197901fcdcde@localhost:5432/monopoly");

	communicationService->setLogger(logger);

	if (!dbConnection->is_open()) {
		logger->error("Failed to open database connection");
		return 1;
	}

	// Declare handlers
	GameRepository gameRepository(communicationService, dbConnection);
	gameRepository.setLogger(logger);

	// Initialize DB schema
	if (!createSchema(dbConnection, logger))
		return 1;

	communicationService->start();

	return 0;
}
