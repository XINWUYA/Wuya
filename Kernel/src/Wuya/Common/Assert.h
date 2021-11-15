#pragma once
#include "debugbreak.h"

#ifdef WUYA_DEBUG
	#define Break() debug_break()
#else
#define Break() void(0)
#endif

#define Assert(expr) { if(!expr) Break();} 
