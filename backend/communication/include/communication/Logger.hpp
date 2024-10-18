#pragma once

#include <format>
#include <string>

class Logger
{
public:
	template <typename... Args>
	void trace(const std::string& fmt, Args&&... args)
	{
		logTrace(std::vformat(fmt, std::make_format_args(args...)));
	}

	template <typename... Args>
	void debug(const std::string& fmt, Args&&... args)
	{
		logDebug(std::vformat(fmt, std::make_format_args(args...)));
	}

	template <typename... Args>
	void info(const std::string& fmt, Args&&... args)
	{
		logInfo(std::vformat(fmt, std::make_format_args(args...)));
	}

	template <typename... Args>
	void warn(const std::string& fmt, Args&&... args)
	{
		logWarn(std::vformat(fmt, std::make_format_args(args...)));
	}

	template <typename... Args>
	void error(const std::string& fmt, Args&&... args)
	{
		logError(std::vformat(fmt, std::make_format_args(args...)));
	}

	template <typename... Args>
	void critical(const std::string& fmt, Args&&... args)
	{
		logCritical(std::vformat(fmt, std::make_format_args(args...)));
	}

protected:
	virtual void logTrace(const std::string& msg) = 0;
	virtual void logDebug(const std::string& msg) = 0;
	virtual void logInfo(const std::string& msg) = 0;
	virtual void logWarn(const std::string& msg) = 0;
	virtual void logError(const std::string& msg) = 0;
	virtual void logCritical(const std::string& msg) = 0;
};
