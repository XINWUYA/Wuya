#pragma once

#ifdef WUYA_EXPORT
	#define WUYA_API __declspec(dllexport)
#else
	#define WUYA_API __declspec(dllimport)
#endif

#define Log(expr) std::cout << (expr) << std::endl
#define LogError(expr) std::cerr << (expr) << std::endl
