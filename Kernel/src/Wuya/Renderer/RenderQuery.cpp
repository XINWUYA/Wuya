#include "Pch.h"
#include "RenderQuery.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLQuery.h"

namespace Wuya
{
	void RenderQueryContext::Init()
	{
		PROFILE_FUNCTION();

		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			break;
		case RenderAPI::OpenGL:
			m_QueryNodes.reserve(DEFAULT_QUERY_COUNT);
			for (uint8_t index = 0; index < DEFAULT_QUERY_COUNT; ++index)
				m_QueryNodes.emplace_back(new OpenGLQueryNode);
			break;
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			break;
		}
	}

	void RenderQueryContext::Destroy()
	{
		PROFILE_FUNCTION();

		for (auto& query : m_QueryNodes)
			delete query;
		
		m_QueryNodes.clear();
	}

	void RenderQueryContext::BeginFrame(uint32_t frame_id)
	{
		PROFILE_FUNCTION();

		m_IsValid = true;
		m_RootNodeIndex = INVALID_QUERY_NODE_INDEX;
		m_CurrentNodeIndex = INVALID_QUERY_NODE_INDEX;
		m_UsedNodeIndex = 0;
		m_FrameIndex = frame_id;
		for (auto& queryNode : m_QueryNodes)
		{
			queryNode->Label = "";
			queryNode->NodeIndex = 0;
			queryNode->ParentNodeIndex = INVALID_QUERY_NODE_INDEX;
			queryNode->ChildrenNodeIndices.clear();
			queryNode->QueryTimeBegin = INVALID_QUERY_TIME;
			queryNode->QueryTimeEnd = INVALID_QUERY_TIME;
			queryNode->ResultTimeBegin = 0.0;
			queryNode->ResultTimeEnd = 0.0;
		}

		BeginGPUScope("FrameTotal");
	}

	void RenderQueryContext::EndFrame()
	{
		PROFILE_FUNCTION();

		while (m_CurrentNodeIndex ^ INVALID_QUERY_NODE_INDEX)
			EndGPUScope();
	}

	void RenderQueryContext::BeginGPUScope(const char* label)
	{
		PROFILE_FUNCTION();

		if (m_UsedNodeIndex >= DEFAULT_QUERY_COUNT)
		{
			m_QueryNodes.emplace_back(new OpenGLQueryNode);
		}

		auto& newQueryNode = m_QueryNodes[m_UsedNodeIndex];
		newQueryNode->NodeIndex = m_UsedNodeIndex++; /* 递增m_UsedNodeIndex */
		newQueryNode->Label = label;

		if (m_CurrentNodeIndex ^ INVALID_QUERY_NODE_INDEX)
		{
			newQueryNode->ParentNodeIndex = m_CurrentNodeIndex;
			auto* currentQueryNode = m_QueryNodes[m_CurrentNodeIndex];
			currentQueryNode->ChildrenNodeIndices.emplace_back(newQueryNode->NodeIndex);
			m_CurrentNodeIndex = newQueryNode->NodeIndex;
		}
		else
		{
			m_RootNodeIndex = m_CurrentNodeIndex = newQueryNode->NodeIndex;
		}

		/* 开始当前Query */
		newQueryNode->Begin();

	}

	void RenderQueryContext::EndGPUScope()
	{
		PROFILE_FUNCTION();

		if (m_CurrentNodeIndex ^ INVALID_QUERY_NODE_INDEX)
		{
			auto* currentQueryNode = m_QueryNodes[m_CurrentNodeIndex];
			currentQueryNode->End();
			m_CurrentNodeIndex = currentQueryNode->ParentNodeIndex;
		}
	}

	bool RenderQueryContext::PrepareQueryResult()
	{
		PROFILE_FUNCTION();

		if (m_RootNodeIndex == INVALID_QUERY_NODE_INDEX)
			return false;

		for (uint32_t i = 0; i < m_UsedNodeIndex; ++i)
		{
			/* 确保所有节点都获取成功 */
			if (!m_QueryNodes[i]->GetQueryResult())
				return false;
		}

		return true;
	}

	RenderQueryProfiler::~RenderQueryProfiler()
	{
		for (auto& context : m_QueryContexts)
			context.Destroy();
	}

	RenderQueryProfiler& RenderQueryProfiler::Instance()
	{
		static RenderQueryProfiler instance;
		return instance;
	}

	void RenderQueryProfiler::BeginFrame(uint32_t frame_id)
	{
		PROFILE_FUNCTION();

		auto& currentContext = m_QueryContexts[m_WriteContextId];
		currentContext.BeginFrame(frame_id);
	}

	void RenderQueryProfiler::EndFrame()
	{
		PROFILE_FUNCTION();

		auto& currentContext = m_QueryContexts[m_WriteContextId];
		currentContext.EndFrame();
	}

	void RenderQueryProfiler::BeginGPUScope(const char* label)
	{
		PROFILE_FUNCTION();

		auto& currentContext = m_QueryContexts[m_WriteContextId];
		currentContext.BeginGPUScope(label);
	}

	void RenderQueryProfiler::EndGPUScope()
	{
		PROFILE_FUNCTION();

		auto& currentContext = m_QueryContexts[m_WriteContextId];
		currentContext.EndGPUScope();
	}

	bool RenderQueryProfiler::PrepareQueryResult(ResultGPUTimerNode& root_node)
	{
		PROFILE_FUNCTION();

		auto& readContext = m_QueryContexts[m_ReadContextId];

		if (readContext.PrepareQueryResult())
		{
			std::function<void(ResultGPUTimerNode&, RenderQueryNode*)> FillResultGPUTimeRecursively;
			FillResultGPUTimeRecursively = [&FillResultGPUTimeRecursively, &readContext](ResultGPUTimerNode& timerNode, RenderQueryNode* queryNode)
				{
					timerNode.Label = queryNode->Label;
					timerNode.QueryIndex = queryNode->NodeIndex;
					timerNode.GPUTime = timerNode.GPUTime * 0.9 + (queryNode->ResultTimeEnd - queryNode->ResultTimeBegin) *0.1;
					const size_t childCount = queryNode->ChildrenNodeIndices.size();
					timerNode.Children.resize(childCount);
					for (size_t i = 0; i < childCount; ++i)
					{
						auto& childTimerNode = timerNode.Children[i];
						auto* childQueryNode = readContext.m_QueryNodes[queryNode->ChildrenNodeIndices[i]];
						FillResultGPUTimeRecursively(childTimerNode, childQueryNode);
					}
				};

			auto* rootQueryNode = readContext.m_QueryNodes[readContext.m_RootNodeIndex];
			FillResultGPUTimeRecursively(root_node, rootQueryNode);

			/* 当一帧数据准备完成时，再切换Context，否则下一帧继续尝试，避免出现刚读完就被刷进新的Query */
			m_WriteContextId = (m_WriteContextId + 1) % MAX_CONTEXTS;
			m_ReadContextId = (m_ReadContextId + 1) % MAX_CONTEXTS;
			return true;
		}

		if (!readContext.m_IsValid)
		{
			/* 当Context未被使用时，也进行切换 */
			m_WriteContextId = (m_WriteContextId + 1) % MAX_CONTEXTS;
			m_ReadContextId = (m_ReadContextId + 1) % MAX_CONTEXTS;
		}
		return false;
	}

	RenderQueryProfiler::RenderQueryProfiler()
	{
		PROFILE_FUNCTION();

		/* 初始化RenderQueryContext */
		m_QueryContexts.resize(MAX_CONTEXTS);
		for (auto& context : m_QueryContexts)
			context.Init();
	}
}
