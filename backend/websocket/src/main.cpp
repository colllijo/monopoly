#include <memory>

#include "Server.hpp"
#include "communication/CommunicationService.hpp"
#include "communication/SpdLogger.hpp"

int main()
{
	std::shared_ptr<SpdLogger> logger = std::make_shared<SpdLogger>();
	std::shared_ptr<Server> server = std::make_shared<Server>();
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();

	communicationService->setLogger(logger);

	server->start();

	return 0;
}
