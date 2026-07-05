#pragma once
#include "Helios/VirtualDevice/DeviceQueryNode.h"

namespace Helios
{
	/* Query Context类
	 * 包含一帧的QueryNode的信息
	 * 提前分配200个，当超过时，再根据需要增加
	 */
	class RenderQueryContext
	{
	public:
		void Init();
		void Destroy();

		void BeginFrame(uint32_t frame_id);
		void EndFrame();

		void BeginGPUScope(const char* label);
		void EndGPUScope();

		bool PrepareQueryResult();

	private:
		static constexpr uint8_t DEFAULT_QUERY_COUNT = 32; // 预留32个QuaryNode
		std::vector<DeviceQueryNode*> m_QueryNodes{};
		uint32_t m_RootNodeIndex{ INVALID_QUERY_NODE_INDEX };
		uint32_t m_CurrentNodeIndex{ INVALID_QUERY_NODE_INDEX };
		uint32_t m_UsedNodeIndex{ 0 };
		uint32_t m_FrameIndex{ 0 };
		bool m_IsValid{ false }; /* 标记当前Context是否已被使用 */

		friend class RenderQueryProfiler;
	};

	/* 收集结果信息 */
	struct ResultGPUTimerNode
	{
		std::string Label{ "" };
		double GPUTime = 0.0;
		uint32_t QueryIndex = INVALID_QUERY_NODE_INDEX;
		std::vector<ResultGPUTimerNode> Children{};
	};

	/*
	 * 用于统计GPUPass耗时信息
	 */
	class RenderQueryProfiler
	{
	public:
		~RenderQueryProfiler();
		/* 单例 */
		static RenderQueryProfiler& Instance();

		void Release();

		void BeginFrame(uint32_t frame_id);
		void EndFrame();

		void BeginGPUScope(const char* label);
		void EndGPUScope();

		bool PrepareQueryResult(ResultGPUTimerNode& root_node);

		/* 开关：控制RenderQuery是否工作 */
		void SetEnabled(bool enabled) { m_IsEnabled = enabled; }
		bool IsEnabled() const { return m_IsEnabled; }

	private:
		RenderQueryProfiler();

		static constexpr uint8_t MAX_CONTEXTS = 4;
		std::vector<RenderQueryContext> m_QueryContexts;
		uint8_t m_ReadContextId{ 0 };
		uint8_t m_WriteContextId{ 1 };
		bool m_IsEnabled{ true }; /* 是否启用GPU耗时统计 */

	};

}
