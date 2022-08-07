#include "Pch.h"
#include "FrameGraph.h"
#include "RenderPassNode.h"
#include "RenderResourceNode.h"

namespace Wuya
{
	/* 使当前节点不会被优化掉，即使没有没使用到 */
	void FrameGraphBuilder::AsSideEffect(bool value)
	{
		m_pRenderPassNode->IsTarget = value;
	}

	/* 根据描述创建一个RenderPass */
	uint32_t FrameGraphBuilder::CreateRenderPass(const std::string& name, const FrameGraphPassInfo::Descriptor& desc)
	{
		return m_pRenderPassNode->CreateRenderPassData(name, desc);
	}

	/* 根据描述创建一个纹理资源 */
	FrameGraphResourceHandleTyped<FrameGraphTexture> FrameGraphBuilder::CreateTexture(const std::string& name, const FrameGraphTexture::Descriptor& desc)
	{
		return m_FrameGraph.CreateResource<FrameGraphTexture>(name, desc);
	}

	/* 获取资源名 */
	const std::string& FrameGraphBuilder::GetResourceName(FrameGraphResourceHandle handle) const
	{
		return m_FrameGraph.GetResource(handle)->GetName();
	}

	FrameGraphBuilder::FrameGraphBuilder(FrameGraph& frame_graph, RenderPassNode* render_pass_node)
		: m_FrameGraph(frame_graph), m_pRenderPassNode(render_pass_node)
	{
	}

	FrameGraph::FrameGraph(const std::string& name)
		: m_DebugName(name)
	{
		PROFILE_FUNCTION();
	}

	FrameGraph::~FrameGraph()
	{
		Destroy();
	}

	/* 获取资源 */
	IResource* FrameGraph::GetResource(FrameGraphResourceHandle handle)
	{
		PROFILE_FUNCTION();

		ASSERT(handle.IsInitialized());

		const auto iter = m_ResourcesMap.find(handle.GetIndex());
		if (iter != m_ResourcesMap.end())
			return iter->second;

		return nullptr;
	}

	/* 获取资源节点 */
	RenderResourceNode* FrameGraph::GetRenderResourceNode(FrameGraphResourceHandle handle)
	{
		const auto iter = m_ResourceToResourceNodeMap.find(handle.GetIndex());
		if (iter != m_ResourceToResourceNodeMap.end())
			return iter->second;

		return nullptr;
	}

	/* 检查一个资源节点是否有效 */
	bool FrameGraph::IsHandleValid(FrameGraphResourceHandle handle) const
	{
		if (!handle.IsInitialized())
			return false;

		// todo: 考虑资源的版本
		return true;
	}

	/* 判断一个资源连线是否有效 */
	bool FrameGraph::IsConnectionValid(const DependencyGraph::Connection* connection) const
	{
		return m_DependencyGraph.IsConnectionValid(connection);
	}

	/* 检查一个FrameGraphPass节点是否被优化掉 */
	bool FrameGraph::IsPassCulled(IFrameGraphPass* pass) const
	{
		return pass->GetRenderPassNode()->IsCulled();
	}

	/* 构建FrameGraph */
	void FrameGraph::Build() noexcept
	{
		PROFILE_FUNCTION();

		CullRenderPassNodes();

		/* 仅对有效的RenderPassNode执行构建 */
		auto iter_current = m_RenderPassNodes.begin();
		while (iter_current != m_LastValidRenderPassNodeIter)
		{
			auto* current_node = *iter_current;

			/* 输入涉及资源 */
			auto incoming_connections = m_DependencyGraph.GetIncomingConnectionsOfNode(current_node);
			for (const auto* connection : incoming_connections)
			{
				auto* from_node = (RenderResourceNode*)m_DependencyGraph.GetNode(connection->FromNodeIdx);
				current_node->RegisterResourceHandle(from_node->GetResourceHandle());
			}

			/* 输出涉及资源 */
			auto outgoing_connections = m_DependencyGraph.GetOutgoingConnectionsOfNode(current_node);
			for (const auto* connection : outgoing_connections)
			{
				auto* to_node = (RenderResourceNode*)m_DependencyGraph.GetNode(connection->ToNodeIdx);
				current_node->RegisterResourceHandle(to_node->GetResourceHandle());
			}
			/* 构建当前节点 */
			current_node->Build();

			/* 下一个 */
			++iter_current;
		}

		/* 确定资源的构建和销毁时机 */
		for (const auto& resource_item : m_ResourcesMap)
		{
			auto* resource = resource_item.second;
			if (resource->GetRefCount() > 0)
			{
				RenderPassNode* first_pass_node = resource->GetFirstPassNode();
				RenderPassNode* last_pass_node = resource->GetLastPassNode();

				if (first_pass_node && last_pass_node)
				{
					ASSERT(first_pass_node->IsValid() && last_pass_node->IsValid());
					/* 资源要在第一次使用时创建 */
					first_pass_node->GetResourcesNeedPrepared().emplace_back(resource);
					/* 在最后一次使用完时销毁 */
					last_pass_node->GetResourcesNeedDestroy().emplace_back(resource);
				}
			}
		}

		/* 在Cull之后再更新资源的Usage */
		for (auto& iter : m_ResourceToResourceNodeMap)
		{
			auto& resource_node = iter.second;
			resource_node->UpdateResourceUsage();
		}
	}

	/* 执行FrameGraph */
	void FrameGraph::Execute() noexcept
	{
		PROFILE_FUNCTION();

		/* 仅执行有效的RenderPassNodes */
		auto iter_current = m_RenderPassNodes.begin();
		while (iter_current != m_LastValidRenderPassNodeIter)
		{
			auto* current_node = *iter_current;

			/* 准备当前节点所需的资源 */
			for (auto* resource : current_node->GetResourcesNeedPrepared())
				resource->Create();

			/* 执行Pass */
			FrameGraphResources resources(*this, *current_node);
			current_node->Execute(resources);

			/* 及时销毁不再使用的资源 */
			for (auto* resource : current_node->GetResourcesNeedDestroy())
				resource->Destroy();

			/* 下一个 */
			++iter_current;
		}
	}

	/* 重置 */
	void FrameGraph::Reset() noexcept
	{
		Destroy();
	}

	/* 导出依赖图 */
	void FrameGraph::ExportGraphviz(const std::string& path)
	{
		const std::string& str = m_DependencyGraph.Graphvizify("FrameGraph");

		std::ofstream file_out(path.c_str());
		file_out << str;
		file_out.close();
	}

	/* 剔除掉无效的RenderPassNode */
	void FrameGraph::CullRenderPassNodes()
	{
		PROFILE_FUNCTION();

		/* 更新依赖图中的引用计数 */
		m_DependencyGraph.UpdateRefCount();

		/* 重新排序并筛选出有效的RenderPassNodes */
		m_LastValidRenderPassNodeIter = std::stable_partition(m_RenderPassNodes.begin(), m_RenderPassNodes.end(),
			[](auto const& render_pass_node)
			{
				return render_pass_node->IsValid();
			});
	}

	/* 添加资源到m_ResourcesMap */
	FrameGraphResourceHandle FrameGraph::AddResourceInternal(IResource* resource)
	{
		const FrameGraphResourceHandle resource_handle(m_ResourcesMap.size());
		m_ResourcesMap.insert({ resource_handle.GetIndex(), resource });

		/* 新增一个RenderResourceNode */
		RenderResourceNode* node = new RenderResourceNode(*this, resource_handle);
		m_ResourceToResourceNodeMap.insert({ resource_handle.GetIndex(), node });

		return resource_handle;
	}

	/* 为RenderPass绑定输入资源 */
	void FrameGraph::BindInputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle,
		const std::function<bool(IResource*, RenderResourceNode*)>& callback)
	{
		if (!IsHandleValid(handle))
			return;

		IResource* resource = GetResource(handle);
		RenderResourceNode* resource_node = GetRenderResourceNode(handle);

		if (!resource_node->HasOutgoingConnection())
		{
			//if (resource_node->)
		}

		/* 连接节点 */
		if (callback(resource, resource_node))
		{

		}
	}

	/* 为RenderPass绑定输出资源 */
	void FrameGraph::BindOutputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle,
		const std::function<bool(IResource*, RenderResourceNode*)>& callback)
	{
		if (!IsHandleValid(handle))
			return;

		IResource* resource = GetResource(handle);
		RenderResourceNode* resource_node = GetRenderResourceNode(handle);

		if (!resource_node->HasIncomingConnection())
		{
			//if (resource_node->)
		}

		/* 连接节点 */
		if (callback(resource, resource_node))
		{
			
		}
	}

	/* 释放FrameGraph */
	void FrameGraph::Destroy()
	{
		for (auto* render_pass_node: m_RenderPassNodes)
			render_pass_node->Destroy();
		m_RenderPassNodes.clear();

		for (const auto& resource_node: m_ResourcesMap)
			resource_node.second->Destroy();
		m_ResourcesMap.clear();

		m_ResourceToResourceNodeMap.clear();

		m_DependencyGraph.Clear();
	}
}
