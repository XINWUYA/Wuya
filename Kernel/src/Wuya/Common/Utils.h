#pragma once

#ifdef WUYA_EXPORT
	#define WUYA_API __declspec(dllexport)
#else
	#define WUYA_API __declspec(dllimport)
#endif
