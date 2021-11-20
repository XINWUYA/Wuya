#pragma once
#include "Wuya/Common/Utils.h"
#include "Wuya/Common/Common.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Wuya 
{
	class WUYA_API Logger
	{
	public:
		static void Init();

		inline static SharedPtr<spdlog::logger>& GetCoreLogger() { return s_pCoreLogger; }
		inline static SharedPtr<spdlog::logger>& GetClientLogger() { return s_pClientLogger; }

	private:
		static SharedPtr<spdlog::logger> s_pCoreLogger;
		static SharedPtr<spdlog::logger> s_pClientLogger;
	};

}

// Related macros to simplify
#define CORE_LOG(...)				::Wuya::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)			::Wuya::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)			::Wuya::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)			::Wuya::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_DEBUG(...)			::Wuya::Logger::GetCoreLogger()->debug(__VA_ARGS__)
#define CORE_LOG_CRITICAL(...)		::Wuya::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define CLIENT_LOG(...)				::Wuya::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_LOG_INFO(...)		::Wuya::Logger::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_LOG_WARN(...)		::Wuya::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_LOG_ERROR(...)		::Wuya::Logger::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_LOG_DEBUG(...)		::Wuya::Logger::GetClientLogger()->debug(__VA_ARGS__)
#define CLIENT_LOG_CRITICAL(...)	::Wuya::Logger::GetClientLogger()->critical(__VA_ARGS__)
