#include "Pch.h"

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

/* Windows内存泄漏检测工具 */
void StartMemoryLeakDetector()
{
#ifdef WUYA_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void EndMemoryLeakDetector()
{
#ifdef WUYA_DEBUG
	_CrtDumpMemoryLeaks();
#endif
}
#else
/* Mac/Linux下暂无内存泄漏检测 */
void StartMemoryLeakDetector() {}
void EndMemoryLeakDetector() {}
#endif

int main(int argc, char** argv)
{
	StartMemoryLeakDetector();

	/* todo: ����argc/argv��ȡ����·�� */

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

	Wuya::Logger::Shutdown();
	EndMemoryLeakDetector();

	return 0;
}
