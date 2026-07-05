#pragma once

namespace Helios
{
	constexpr uint32_t INVALID_QUERY_NODE_INDEX = UINT32_MAX;
	constexpr uint64_t INVALID_QUERY_TIME = UINT64_MAX;

	/* RenderQueryNode基类
	 * 用于记录一个GPU区间的一对时间戳
	 */
	struct DeviceQueryNode
	{
		virtual ~DeviceQueryNode() = default;

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

		static DeviceQueryNode* Create();
	};
}