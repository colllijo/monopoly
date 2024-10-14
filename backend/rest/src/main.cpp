#include <crow.h>
#include "crow/middlewares/cors.h"

#include <memory>

#include "CrowLogger.hpp"
#include "GameController.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"
#include "crow/logging.h"

int main()
{
	const auto logger = std::make_shared<SpdLogger>();
	const auto crowLogger = std::make_shared<CrowLogger>(logger);
	const auto app = std::make_shared<crow::App<crow::CORSHandler>>();
	const auto communicationService = std::make_shared<CommunicationService>();

	crow::logger::setHandler(crowLogger.get());
	communicationService->setLogger(logger);

	// Create Controllers
	GameController gameController(app, communicationService);
	gameController.setLogger(logger);

	logger->info("Starting the RestService");
	communicationService->start_async();
	app->port(8080).multithreaded().run();

	communicationService->stop();

	return 0;
}
