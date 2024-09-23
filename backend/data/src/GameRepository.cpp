#include "GameRepository.hpp"
#include "communication/Command.hpp"
#include "communication/CommunicationService.hpp"
#include <iostream>
#include <memory>

GameRepository::GameRepository(std::shared_ptr<CommunicationService> communication) : communication(communication) {
	communication->handleCommand(communication::commands::GetGames(), [](communication::Command) {
		std::cout << "GetGames command received" << std::endl;

		return nlohmann::json::parse("{\"games\": []}");
	});
}
