#include "Pch.h"
#include "FrameGraph.h"
#include "RenderPassNode.h"
#include "RenderResourceNode.h"

namespace Wuya
{
	/* ������������һ��RenderPass */
	uint32_t FrameGraphBuilder::CreateRenderPass(const std::string& name, const FrameGraphPassInfo::Descriptor& desc)
	{
		return m_pRenderPassNode->CreateRenderPassData(name, desc);
	}

	/* ������������һ��������Դ */
	FrameGraphResourceHandleTyped<FrameGraphTexture> FrameGraphBuilder::CreateTexture(const std::string& name, const FrameGraphTexture::Descriptor& desc)
	{
		return m_FrameGraph.CreateResource<FrameGraphTexture>(name, desc);
	}

	/* ʹ��ǰ�ڵ㲻�ᱻ�Ż�������ʹû��ûʹ�õ� */
	void FrameGraphBuilder::AsSideEffect(bool value)
	{
		m_pRenderPassNode->IsTarget = value;
	}

	/* ������ԴHandle��FrameGraph�л�ȡ��Դ�� */
	const std::string& FrameGraphBuilder::GetResourceName(FrameGraphResourceHandle handle) const
	{
		return m_FrameGraph.GetResource(handle)->GetName();
	}

	FrameGraphBuilder::FrameGraphBuilder(FrameGraph& frame_graph, const SharedPtr<RenderPassNode>& render_pass_node)
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

	/* ��ȡ��Դ */
	SharedPtr<IResource> FrameGraph::GetResource(FrameGraphResourceHandle handle)
	{
		PROFILE_FUNCTION();

		if (!handle.IsInitialized())
			return nullptr;

		const auto iter = m_ResourcesMap.find(handle.GetIndex());
		if (iter != m_ResourcesMap.end())
			return iter->second;

		return nullptr;
	}

	/* ��ȡ��Դ�ڵ� */
	SharedPtr<RenderResourceNode> FrameGraph::GetRenderResourceNode(FrameGraphResourceHandle handle)
	{
		const auto iter = m_ResourceToResourceNodeMap.find(handle.GetIndex());
		if (iter != m_ResourceToResourceNodeMap.end())
			return iter->second;

		return nullptr;
	}

	/* ���һ����Դ�ڵ��Ƿ���Ч */
	bool FrameGraph::IsHandleValid(FrameGraphResourceHandle handle) const
	{
		if (!handle.IsInitialized())
			return false;

		// todo: ������Դ�İ汾
		return true;
	}

	/* �ж�һ����Դ�����Ƿ���Ч */
	bool FrameGraph::IsConnectionValid(const SharedPtr<DependencyGraph::Connection>& connection) const
	{
		return m_DependencyGraph.IsConnectionValid(connection);
	}

	/* ���һ��FrameGraphPass�ڵ��Ƿ��Ż��� */
	bool FrameGraph::IsPassCulled(const SharedPtr<IFrameGraphPass>& pass) const
	{
		return pass->GetRenderPassNode()->IsCulled();
	}

	/* ����FrameGraph */
	void FrameGraph::Build() noexcept
	{
		PROFILE_FUNCTION();

		CullRenderPassNodes();

		/* ������Ч��RenderPassNodeִ�й��� */
		auto iter_current = m_RenderPassNodes.begin();
		while (iter_current != m_LastValidRenderPassNodeIter)
		{
			auto current_node = *iter_current;

			/* �����漰��Դ */
			auto incoming_connections = m_DependencyGraph.GetIncomingConnectionsOfNode(current_node);
			for (const auto& connection : incoming_connections)
			{
				const auto from_node = DynamicPtrCast<const RenderResourceNode>(m_DependencyGraph.GetNode(connection->FromNodeIdx));
				current_node->RegisterResourceHandle(from_node->GetResourceHandle());
			}

			/* ����漰��Դ */
			auto outgoing_connections = m_DependencyGraph.GetOutgoingConnectionsOfNode(current_node);
			for (const auto& connection : outgoing_connections)
			{
				const auto to_node = DynamicPtrCast<const RenderResourceNode>(m_DependencyGraph.GetNode(connection->ToNodeIdx));
				current_node->RegisterResourceHandle(to_node->GetResourceHandle());
			}
			/* ������ǰ�ڵ� */
			current_node->Build();

			/* ��һ�� */
			++iter_current;
		}

		/* ȷ����Դ�Ĺ���������ʱ�� */
		for (const auto& resource_item : m_ResourcesMap)
		{
			auto& resource = resource_item.second;
			if (resource->GetRefCount() > 0)
			{
				auto& first_pass_node = resource->GetFirstPassNode();
				auto& last_pass_node = resource->GetLastPassNode();

				if (first_pass_node && last_pass_node)
				{
					ASSERT(first_pass_node->IsValid() && last_pass_node->IsValid());
					/* ��ԴҪ�ڵ�һ��ʹ��ʱ���� */
					first_pass_node->GetResourcesNeedPrepared().emplace_back(resource);
					/* �����һ��ʹ����ʱ���� */
					last_pass_node->GetResourcesNeedDestroy().emplace_back(resource);
				}
			}
		}

		/* ��Cull֮���ٸ�����Դ��Usage */
		for (auto& iter : m_ResourceToResourceNodeMap)
		{
			auto& resource_node = iter.second;
			resource_node->UpdateResourceUsage();
		}
	}

	/* ִ��FrameGraph */
	void FrameGraph::Execute() noexcept
	{
		PROFILE_FUNCTION();

		if (m_RenderPassNodes.empty())
			return;

		/* ��ִ����Ч��RenderPassNodes */
		auto iter_current = m_RenderPassNodes.begin();
		while (iter_current != m_LastValidRenderPassNodeIter)
		{
			auto& current_node = *iter_current;

			/* ׼����ǰ�ڵ��������Դ */
			for (auto& resource : current_node->GetResourcesNeedPrepared())
				resource->Create();

			/* ִ��Pass */
			FrameGraphResources resources(*this, *current_node);
			current_node->Execute(resources);

			/* ��ʱ���ٲ���ʹ�õ���Դ, ���Target Pass����Դ�ݲ��ͷţ���ΪImGui��Ҫ�õ���todo:ֻ����������ͷţ�����Ӧ�����ͷ� */
			if (!current_node->IsTarget)
			{
				for (auto& resource : current_node->GetResourcesNeedDestroy())
					resource->Destroy();
			}
			/* ��һ�� */
			++iter_current;
		}
	}

	/* ���� */
	void FrameGraph::Reset() noexcept
	{
		Destroy();
	}

	/* ��������ͼ */
	void FrameGraph::ExportGraphviz(const std::string& path)
	{
		const std::string& str = m_DependencyGraph.Graphvizify("FrameGraph");

		std::ofstream file_out(path.c_str());
		file_out << str;
		file_out.close();
	}

	/* �޳�����Ч��RenderPassNode */
	void FrameGraph::CullRenderPassNodes()
	{
		PROFILE_FUNCTION();

		/* ��������ͼ�е����ü��� */
		m_DependencyGraph.UpdateRefCount();

		/* ��������ɸѡ����Ч��RenderPassNodes */
		m_LastValidRenderPassNodeIter = std::stable_partition(m_RenderPassNodes.begin(), m_RenderPassNodes.end(),
			[](auto const& render_pass_node)
			{
				return render_pass_node->IsValid();
			});
	}

	/* �����Դ��m_ResourcesMap */
	FrameGraphResourceHandle FrameGraph::AddResourceInternal(const SharedPtr<IResource>& resource)
	{
		const FrameGraphResourceHandle resource_handle(m_ResourcesMap.size());
		m_ResourcesMap.insert({ resource_handle.GetIndex(), resource });

		/* ����һ��RenderResourceNode */
		auto resource_node = CreateSharedPtr<RenderResourceNode>(resource->GetName(), *this, resource_handle);
		/* ����Դ�ڵ���ӵ�����ͼ */
		m_DependencyGraph.RegisterNode(resource_node);
		m_ResourceToResourceNodeMap.insert({ resource_handle.GetIndex(), resource_node });

		return resource_handle;
	}

	/* ΪRenderPass��������Դ */
	void FrameGraph::BindInputResourceInternal(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandle handle,
		const std::function<bool(const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)>& callback)
	{
		if (!IsHandleValid(handle))
			return;

		const SharedPtr<IResource>& resource = GetResource(handle);
		auto& resource_node = GetRenderResourceNode(handle);

		if (!resource_node->HasOutgoingConnection())
		{
			//if (resource_node->)
		}

		/* ���ӽڵ� */
		if (callback(resource, resource_node))
		{

		}
	}

	/* ΪRenderPass�������Դ */
	void FrameGraph::BindOutputResourceInternal(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandle handle,
		const std::function<bool(const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)>& callback)
	{
		if (!IsHandleValid(handle))
			return;

		const SharedPtr<IResource>& resource = GetResource(handle);
		auto& resource_node = GetRenderResourceNode(handle);

		if (!resource_node->HasIncomingConnection())
		{
			//if (resource_node->)
		}

		/* ���ӽڵ� */
		if (callback(resource, resource_node))
		{
			
		}
	}

	/* �ͷ�FrameGraph */
	void FrameGraph::Destroy()
	{
		for (const auto& render_pass_node: m_RenderPassNodes)
			render_pass_node->Destroy();
		m_RenderPassNodes.clear();

		/* ����������ResourceNode����������Resource */
		for (const auto& resource_node_itr : m_ResourceToResourceNodeMap)
			resource_node_itr.second->Destroy();
		m_ResourceToResourceNodeMap.clear();

		for (const auto& resource_node: m_ResourcesMap)
			resource_node.second->Destroy();
		m_ResourcesMap.clear();

		m_DependencyGraph.Clear();
	}
}
