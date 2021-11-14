#pragma once
#include "Wuya/Common/Utils.h"
#include "spdlog/spdlog.h"

namespace Wuya 
{
	class WUYA_API Logger
	{
	public:
		using SharedPtr = std::shared_ptr<spdlog::logger>;

		static void Init();

		inline static SharedPtr& GetCoreLogger() { return s_pCoreLogger; }
		inline static SharedPtr& GetClientLogger() { return s_pClientLogger; }

	private:
		static SharedPtr s_pCoreLogger;
		static SharedPtr s_pClientLogger;
	};

}

// Related macros to simplify
#define CORE_LOG(...)			::Wuya::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)		::Wuya::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)		::Wuya::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)		::Wuya::Logger::GetCoreLogger()->error(__VA_ARGS__)

#define CLIENT_LOG(...)			::Wuya::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_LOG_INFO(...)	::Wuya::Logger::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_LOG_WARN(...)	::Wuya::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_LOG_ERROR(...)	::Wuya::Logger::GetClientLogger()->error(__VA_ARGS__)
