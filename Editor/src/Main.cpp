#include "Pch.h"
#include <Kernel.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

/* 启用内存泄漏检测工具 */
void InitMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#ifdef WUYA_DEBUG
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 251606;
#endif
}

int main(int argc, char** argv)
{
	InitMemoryLeakDetector();

	/* todo: 根据argc/argv获取工程路径 */

	Wuya::Logger::Init();
	EDITOR_LOG("Wuya Kernel 1.0.0");

	// See the profiler result in: edge://tracing/
	// More suggested: https://ui.perfetto.dev/
	// Just Drag the json file in.
	PROFILER_BEGIN_SESSION("Startup", "TimeCostProfiler-Startup.json");
	auto app = Wuya::CreateApplication();
	PROFILER_END_SESSION();

	PROFILER_BEGIN_SESSION("Runtime", "TimeCostProfiler-Runtime.json");
	app->Run();
	PROFILER_END_SESSION();

	_CrtDumpMemoryLeaks();
	return 0;
}
