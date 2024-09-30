#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

#include "Logger.hpp"
#include "spdlog/common.h"

class SpdLogger : public Logger
{
public:
	SpdLogger()
	{
		auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		stdout_sink->set_level(spdlog::level::info);

		auto stderr_sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
		stderr_sink->set_level(spdlog::level::err);

		std::vector<spdlog::sink_ptr> sinks{stdout_sink, stderr_sink};
		logger = std::make_unique<spdlog::logger>("multi_sink_logger", sinks.begin(), sinks.end());

		logger->set_level(spdlog::level::info);
		logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
	}

protected:
	void logTrace(const std::string &msg) override { logger->trace(msg); }

	void logDebug(const std::string &msg) override { logger->debug(msg); }

	void logInfo(const std::string &msg) override { logger->info(msg); }

	void logWarn(const std::string &msg) override { logger->warn(msg); }

	void logError(const std::string &msg) override { logger->error(msg); }

	void logCritical(const std::string &msg) override { logger->critical(msg); }

private:
	std::unique_ptr<spdlog::logger> logger;
};
