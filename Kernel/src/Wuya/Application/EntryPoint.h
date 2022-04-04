#pragma once

extern Wuya::UniquePtr<Wuya::Application> Wuya::CreateApplication();

int main(int argc, char** argv)
{
	Wuya::Logger::Init();
	CORE_LOG("Wuya Kernel 1.0.0");

	// See the profiler result in: edge://tracing/
	PROFILER_BEGIN_SESSION("Startup", "TimeCostProfiler-Startup.json");
	auto app = Wuya::CreateApplication();
	PROFILER_END_SESSION();

	PROFILER_BEGIN_SESSION("Runtime", "TimeCostProfiler-Runtime.json");
	app->Run();
	PROFILER_END_SESSION();

	return 0;
}
