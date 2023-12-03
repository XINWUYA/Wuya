#pragma once
namespace Wuya
{
	constexpr uint32_t INVALID_QUERY_NODE_INDEX = UINT32_MAX;
	constexpr uint64_t INVALID_QUERY_TIME = UINT64_MAX;

	/* RenderQueryNode基类
	 * 用于记录一个GPU区间的一对时间戳
	 */
	struct RenderQueryNode
	{
		virtual ~RenderQueryNode() = default;

		virtual void Begin() {}
		virtual void End() {}
		virtual bool GetQueryResult() { return true; }

		std::string Label{ "Unnamed QueryNode" };
		uint32_t NodeIndex{ 0 };
		uint32_t ParentNodeIndex{ INVALID_QUERY_NODE_INDEX };
		std::vector<uint32_t> ChildrenNodeIndices{};
		uint64_t QueryTimeBegin = INVALID_QUERY_TIME;
		uint64_t QueryTimeEnd = INVALID_QUERY_TIME;

		double ResultTimeBegin = 0.0;
		double ResultTimeEnd = 0.0;
	};

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
		static constexpr uint8_t DEFAULT_QUERY_COUNT = 200;
		std::vector<RenderQueryNode*> m_QueryNodes{};
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

		void BeginFrame(uint32_t frame_id);
		void EndFrame();

		void BeginGPUScope(const char* label);
		void EndGPUScope();

		bool PrepareQueryResult(ResultGPUTimerNode& root_node);

	private:
		RenderQueryProfiler();

		static constexpr uint8_t MAX_CONTEXTS = 4;
		std::vector<RenderQueryContext> m_QueryContexts;
		uint8_t m_ReadContextId{ 0 };
		uint8_t m_WriteContextId{ 1 };

	};

}
