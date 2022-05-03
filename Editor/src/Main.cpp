#include "Pch.h"
#include <Kernel.h>

int main(int argc, char** argv)
{
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

	return 0;
}
