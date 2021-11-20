#pragma once
#include "Wuya/Common/Common.h"
#include "Wuya/Core/Logger.h"
#include <filesystem>
#include "debugbreak.h"

#ifdef WUYA_DEBUG
	#define Break() debug_break()
#else
#define Break() void(0)
#endif

#define ASSERT_IMPL(expr, msg, ...) { if(!expr) { CORE_LOG_ERROR(msg, __VA_ARGS__); Break(); } } 
#define ASSERT_WITH_MSG(expr, ...) ASSERT_IMPL(expr, "Assertion failed:{}", __VA_ARGS__)
#define ASSERT_NO_MSG(expr) ASSERT_IMPL(expr, "Assertion({0}) failed at: {1}:{2}", STRINGIFY(expr), std::filesystem::path(__FILE__).filename().string(), __LINE__)

// 如果只有一个参数，则name将被替换为ASSERT_NO_MSG； 如果有两个参数，name将被替换为ASSERT_WITH_MSG。
#define ASSERT_AUTO_SELECT_NAME(arg1, arg2, name, ...) name
#define ASSERT_AUTO_SELECT(...) EXPAND_MACRO(ASSERT_AUTO_SELECT_NAME(__VA_ARGS__, ASSERT_WITH_MSG, ASSERT_NO_MSG))

#define CORE_ASSERT(...) EXPAND_MACRO(ASSERT_AUTO_SELECT(__VA_ARGS__)(__VA_ARGS__))
