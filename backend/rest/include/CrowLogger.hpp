#pragma once

#include <memory>

#include "communication/Logger.hpp"
#include "crow/logging.h"
class CrowLogger : public crow::ILogHandler
{
public:
	CrowLogger(const std::shared_ptr<Logger> logger) : logger(logger) {}

	void log(std::string message, crow::LogLevel level)
	{
		switch (level)
		{
			case crow::LogLevel::DEBUG:
				logger->debug(message);
				break;
			case crow::LogLevel::INFO:
				logger->info(message);
				break;
			case crow::LogLevel::WARNING:
				logger->warn(message);
				break;
			case crow::LogLevel::ERROR:
				logger->error(message);
				break;
			case crow::LogLevel::CRITICAL:
				logger->critical(message);
				break;
		}
	}

private:
	std::shared_ptr<Logger> logger;
};
