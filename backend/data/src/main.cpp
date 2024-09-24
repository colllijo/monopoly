#include <memory>
#include <pqxx/pqxx>

#include "GameRepository.hpp"
#include "Database.hpp"
#include "communication/CommunicationService.hpp"

int main() {
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();
	std::shared_ptr<pqxx::connection> dbConnection = std::make_shared<pqxx::connection>("postgresql://pennybags:131044f9-0b69-4031-aad4-197901fcdcde@localhost:5432/monopoly");

	if (!dbConnection->is_open()) {
		std::cerr << "Failed to connect to database" << std::endl;
		return 1;
	}

	// Declare handlers
	GameRepository gameRepository(communicationService, dbConnection);

	// Initialize DB schema
	try {
		createSchema(dbConnection);
	} catch (const pqxx::sql_error &e) {
		return 1;
	}

	std::cout << "Starting communication service" << std::endl;
	communicationService->start();

	return 0;
}
