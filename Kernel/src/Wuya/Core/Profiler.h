#pragma once
#include <mutex>
#include <fstream>

namespace Wuya
{
	struct ProfileResult
	{
		std::string Name; // 标记名
		std::chrono::duration<double, std::micro> StartTime; // 开始时间
		std::chrono::microseconds ElapsedTime; // 耗时
		std::thread::id ThreadId; // 所属线程
	};

	// 耗时分析
	class TimeCostProfiler
	{
	public:
		TimeCostProfiler(const TimeCostProfiler&) = delete;
		TimeCostProfiler(TimeCostProfiler&&) = delete;

		static TimeCostProfiler* Instance();

		void BeginSession(const std::string& name, const std::string& outfile = "ProfileResult-TimeCost.json");
		void EndSession();

		void WriteProfileResult(const ProfileResult& result);

	private:
		TimeCostProfiler() = default;
		~TimeCostProfiler();

		void WriteHeader();
		void WriteFooter();

		void InternalEndSession();

		struct SessionInfo
		{
			std::string Name;
		};

		static TimeCostProfiler* s_pInstance;

		std::mutex m_Mutex{};
		SharedPtr<SessionInfo> m_pSessionInfo{ nullptr };
		std::ofstream m_OutputStream{};
	};

	// 分析计时器
	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char* name);
		~ProfilerTimer();

		void Stop();

	private:
		const char* m_pName;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint{};
		bool m_Stopped{ false };
	};

	namespace ProfilerUtils
	{
		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}

}

// See the profiler result in: edge://tracing/
// More suggested: https://ui.perfetto.dev/
// Just Drag the json file in.
#define WUYA_PROFILE 0

#if WUYA_PROFILE
	// Resolve which function signature macro will be used. Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define FUNC_SIG __func__
	#else
		#define FUNC_SIG "FUNC_SIG unknown!"
	#endif

	// 开启一个耗时分析
	#define PROFILER_BEGIN_SESSION(name, filepath) ::Wuya::TimeCostProfiler::Instance()->BeginSession(name, filepath)
	// 关闭一个耗时分析
	#define PROFILER_END_SESSION() ::Wuya::TimeCostProfiler::Instance()->EndSession()
	// 分析当前指令段耗时
	#define PROFILE_SCOPE_LINE(name, line) constexpr auto fixed_name_##line = ::Wuya::ProfilerUtils::CleanupOutputString(name, "__cdecl "); /*去掉前缀*/\
											::Wuya::ProfilerTimer timer_##line(fixed_name_##line.Data)
	#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
	// 分析当前函数耗时
	#define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#else
	#define PROFILER_BEGIN_SESSION(name, filepath)
	#define PROFILER_END_SESSION()
	#define PROFILE_SCOPE(name)
	#define PROFILE_FUNCTION()
#endif