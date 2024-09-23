#include <memory>

#include "GameRepository.hpp"
#include "communication/CommunicationService.hpp"

int main() {
	std::shared_ptr<CommunicationService> communicationService = std::make_shared<CommunicationService>();

	// Declare handlers
	GameRepository gameRepository(communicationService);

	communicationService->start();

	return 0;
}
