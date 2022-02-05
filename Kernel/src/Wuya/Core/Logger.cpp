#include "pch.h"
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Wuya
{
	SharedPtr<spdlog::logger> Logger::s_pCoreLogger;
	SharedPtr<spdlog::logger> Logger::s_pClientLogger;

	void Logger::Init()
	{
		std::vector<spdlog::sink_ptr> log_sinks;
		log_sinks.emplace_back(CreateSharedPtr<spdlog::sinks::stdout_color_sink_mt>());
		log_sinks.emplace_back(CreateSharedPtr<spdlog::sinks::basic_file_sink_mt>("Wuya.log", true));
		// 指定Log样式
		log_sinks[0]->set_pattern("%^[%T] [thread %t] %n: %v%$");
		log_sinks[1]->set_pattern("[%T] [thread %t] [%l] %n: %v");

		s_pCoreLogger = CreateSharedPtr<spdlog::logger>("Wuya Kernel", std::begin(log_sinks), std::end(log_sinks));
		spdlog::register_logger(s_pCoreLogger);
		s_pCoreLogger->set_level(spdlog::level::trace);
		s_pCoreLogger->flush_on(spdlog::level::trace);

		s_pClientLogger = CreateSharedPtr<spdlog::logger>("Terminator", std::begin(log_sinks), std::end(log_sinks));
		spdlog::register_logger(s_pClientLogger);
		s_pClientLogger->set_level(spdlog::level::trace);
		s_pClientLogger->flush_on(spdlog::level::trace);

	}
}