#include <memory>

#include "Server.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main()
{
	const auto logger = std::make_shared<SpdLogger>();
	const auto communicationService = std::make_shared<CommunicationService>(logger);
	const auto server = std::make_shared<Server>(communicationService);

	server->setLogger(logger);

	communicationService->start_async();
	server->start();

	return 0;
}
