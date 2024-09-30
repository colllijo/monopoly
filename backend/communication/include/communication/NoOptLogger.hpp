#pragma once

#include "Logger.hpp"

class NoOptLogger : public Logger
{
public:
	NoOptLogger() {}

protected:
	void logTrace(const std::string &) override {}

	void logDebug(const std::string &) override {}

	void logInfo(const std::string &) override {}

	void logWarn(const std::string &) override {}

	void logError(const std::string &) override {}

	void logCritical(const std::string &) override {}
};
