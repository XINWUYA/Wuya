#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "Wuya/Common/Common.h"

namespace Wuya 
{
	class Logger
	{
	public:
		static void Init();
		static void Shutdown();

		inline static SharedPtr<spdlog::logger>& GetCoreLogger() { return s_pCoreLogger; }
		inline static SharedPtr<spdlog::logger>& GetEditorLogger() { return s_pEditorLogger; }

	private:
		static SharedPtr<spdlog::logger> s_pCoreLogger;
		static SharedPtr<spdlog::logger> s_pEditorLogger;
	};

}

// Related macros to simplify
#define CORE_LOG(...)				::Wuya::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)			::Wuya::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)			::Wuya::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)			::Wuya::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_DEBUG(...)			::Wuya::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define CORE_LOG_CRITICAL(...)		::Wuya::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define EDITOR_LOG(...)				::Wuya::Logger::GetEditorLogger()->trace(__VA_ARGS__)
#define EDITOR_LOG_INFO(...)		::Wuya::Logger::GetEditorLogger()->info(__VA_ARGS__)
#define EDITOR_LOG_WARN(...)		::Wuya::Logger::GetEditorLogger()->warn(__VA_ARGS__)
#define EDITOR_LOG_ERROR(...)		::Wuya::Logger::GetEditorLogger()->error(__VA_ARGS__)
#define EDITOR_LOG_DEBUG(...)		::Wuya::Logger::GetEditorLogger()->debug(__VA_ARGS__)
#define EDITOR_LOG_CRITICAL(...)	::Wuya::Logger::GetEditorLogger()->critical(__VA_ARGS__)
