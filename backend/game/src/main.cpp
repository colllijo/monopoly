#include <memory>

#include "GameService.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main() {
	std::shared_ptr<SpdLogger> logger = std::make_shared<SpdLogger>();
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();

	communicationService->setLogger(logger);

	// Declare handlers
	GameService gameService(communicationService);
	gameService.setLogger(logger);

	logger->info("Starting the GameService");
	communicationService->start();

	return 0;
}
