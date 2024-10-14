#include <memory>

#include "Server.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main()
{
	const auto logger = std::make_shared<SpdLogger>();
	const auto server = std::make_shared<Server>();
	const auto communicationService = std::make_shared<CommunicationService>();

	communicationService->setLogger(logger);

	server->start();

	return 0;
}
