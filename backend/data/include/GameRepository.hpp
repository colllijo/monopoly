#pragma once

#include "communication/CommunicationService.hpp"
#include <memory>
class GameRepository {
public:
	GameRepository(std::shared_ptr<CommunicationService> communication);

	private:
	std::shared_ptr<CommunicationService> communication;
};
