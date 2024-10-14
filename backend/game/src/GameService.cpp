#include "GameService.hpp"

#include "communication/NoOptLogger.hpp"

GameService::GameService(std::shared_ptr<CommunicationService> communication) : logger(NoOptLogger::getInstance()), communication(communication)
{
}
