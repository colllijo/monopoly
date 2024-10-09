#pragma once

#include <memory>
#include "Logger.hpp"

class NoOptLogger : public Logger
{
public:
	static std::shared_ptr<Logger> getInstance()
	{
		static std::shared_ptr<NoOptLogger> instance(new NoOptLogger());
		return instance;
	}

	NoOptLogger(const NoOptLogger &) = delete;
	void operator=(const NoOptLogger &) = delete;

protected:
	void logTrace(const std::string &) override {}

	void logDebug(const std::string &) override {}

	void logInfo(const std::string &) override {}

	void logWarn(const std::string &) override {}

	void logError(const std::string &) override {}

	void logCritical(const std::string &) override {}

private:
	NoOptLogger() = default;
};
