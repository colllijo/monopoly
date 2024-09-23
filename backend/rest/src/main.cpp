#include <crow.h>
#include <memory>

#include "GameController.hpp"
#include "communication/CommunicationService.hpp"

int main() {
    std::shared_ptr<crow::SimpleApp> app = std::make_shared<crow::SimpleApp>();
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();

	// Create Controllers
    GameController gameController(app, communicationService);

	communicationService->start_async();
    app->port(8080).multithreaded().run();

	communicationService->stop();

	return 0;
}

