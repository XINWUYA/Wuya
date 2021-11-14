#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Wuya
{
	Logger::SharedPtr Logger::s_pCoreLogger;
	Logger::SharedPtr Logger::s_pClientLogger;

	void Logger::Init()
	{
		// 指定Log样式
		spdlog::set_pattern("%^[%T] [thread %t] %n: %v%$");

		s_pCoreLogger = spdlog::stdout_color_mt("Wuya");
		s_pCoreLogger->set_level(spdlog::level::trace);

		s_pClientLogger = spdlog::stdout_color_mt("Terminator");
		s_pClientLogger->set_level(spdlog::level::trace);

	}
}