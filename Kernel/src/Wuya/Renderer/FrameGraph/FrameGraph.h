#pragma once
#include "Blackboard.h"
#include "FrameGraphPass.h"
#include "DependencyGraph.h"
#include "FrameGraphResourceHandle.h"
#include "FrameGraphResources.h"
#include "Resource.h"
#include "RenderPassNode.h"
#include "RenderResourceNode.h"

namespace Wuya
{
	class FrameGraph;
	class RenderResourceNode;

	/* FrameGraph Builder��
	 * ÿ��RenderPass��Ӧ��Ӧһ��Builder
	 * ���ڴ�����󶨵�ǰRenderPass�������Դ
	 */
	class FrameGraphBuilder
	{
	public:
		explicit FrameGraphBuilder(FrameGraphBuilder&) = delete;
		FrameGraphBuilder& operator=(const FrameGraphBuilder&) = delete;
		~FrameGraphBuilder() = default;

		/* ������������RenderPass */
		uint32_t CreateRenderPass(const std::string& name, const FrameGraphPassInfo::Descriptor& desc = {});

		/* ������������һ��������Դ */
		FrameGraphResourceHandleTyped<FrameGraphTexture> CreateTexture(const std::string& name, const FrameGraphTexture::Descriptor& desc = {});

		/* Ϊ��ǰRenderPass��������Դ */
		template<typename ResourceType>
		void BindInputResource(FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultReadUsage)
		{
			m_FrameGraph.BindInputResource<ResourceType>(m_pRenderPassNode, handle, usage);
		}

		/* Ϊ��ǰRenderPass�������Դ */
		template<typename ResourceType>
		void BindOutputResource(FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultWriteUsage)
		{
			m_FrameGraph.BindOutputResource<ResourceType>(m_pRenderPassNode, handle, usage);
		}

		/* ʹ��ǰ�ڵ㲻�ᱻ�Ż�������ʹû��ûʹ�õ� */
		void AsSideEffect(bool value = true);

		/* ������ԴHandle��FrameGraph�л�ȡ��Դ�� */
		[[nodiscard]] const std::string& GetResourceName(FrameGraphResourceHandle handle) const;


	private:
		FrameGraphBuilder(FrameGraph& frame_graph, const SharedPtr<RenderPassNode>& render_pass_node);

		/* ��ǰBuilder������FrameGraph */
		FrameGraph& m_FrameGraph;
		/* ��Ӧ��RenderPass */
		SharedPtr<RenderPassNode> m_pRenderPassNode{ nullptr };

		friend class FrameGraph;
	};

	/* FrameGraph�� */
	class FrameGraph final
	{
	public:
		explicit FrameGraph(const std::string& name);
		FrameGraph(FrameGraph&) = delete;
		FrameGraph& operator=(const FrameGraph&) = delete;
		~FrameGraph();

		/* ���һ��FrameGraphPass��
		 * SetupFunc��������ִ�У�
		 * ExecuteFunc����ִ������FrameGraphʱ�Ż�ִ�У�
		 */
		template<typename Data, typename SetupFunc, typename ExecuteFunc>
		SharedPtr<FrameGraphPass<Data, ExecuteFunc>> AddPass(const std::string& name, SetupFunc setup_func, ExecuteFunc&& execute_func)
		{
			auto frame_graph_pass = CreateSharedPtr<FrameGraphPass<Data, ExecuteFunc>>(std::forward<ExecuteFunc>(execute_func));
			auto render_pass_node = CreateSharedPtr<RenderPassNode>(name, *this, frame_graph_pass);
			/* ��Pass�ڵ�ע�ᵽ����ͼ�� */
			m_DependencyGraph.RegisterNode(render_pass_node);

			frame_graph_pass->SetRenderPassNode(render_pass_node);
			m_RenderPassNodes.emplace_back(render_pass_node);

			FrameGraphBuilder builder(*this, render_pass_node);

			/* ִ��Setup�׶� */
			setup_func(builder, const_cast<Data&>(frame_graph_pass->GetData()));

			return frame_graph_pass;
		}

		/* ������Դ */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> CreateResource(const std::string& name, const typename ResourceType::Descriptor& desc = {})
		{
			auto resource = CreateSharedPtr<Resource<ResourceType>>(name, desc);
			return FrameGraphResourceHandleTyped<ResourceType>(AddResourceInternal(resource));
		}

		/* ΪRenderPass��������Դ */
		template<typename ResourceType>
		void BindInputResource(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultReadUsage)
		{
			BindInputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](const SharedPtr<IResource>& resource, const SharedPtr<RenderResourceNode>& resource_node)
				{
					/* ����Դ�ڵ����ӵ�����ͼ�� */
					auto res = DynamicPtrCast<Resource<ResourceType>>(resource);
					return res->ConnectAsInput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* ΪRenderPass�������Դ */
		template<typename ResourceType>
		void BindOutputResource(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultWriteUsage)
		{
			BindOutputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](const SharedPtr<IResource>& resource, const SharedPtr<RenderResourceNode>& resource_node)
				{
					/* ����Դ�ڵ����ӵ�����ͼ�� */
					auto res = DynamicPtrCast<Resource<ResourceType>>(resource);
					return res->ConnectAsOutput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* ��ȡ��Դ */
		[[nodiscard]] SharedPtr<IResource> GetResource(FrameGraphResourceHandle handle);
		/* ��ȡ��Դ�ڵ� */
		[[nodiscard]] SharedPtr<RenderResourceNode> GetRenderResourceNode(FrameGraphResourceHandle handle);
		/* ��ȡ������ϵͼ */
		[[nodiscard]] DependencyGraph& GetDependencyGraph() { return m_DependencyGraph; }
		/* ��ȡBlackboard */
		[[nodiscard]] Blackboard& GetBlackboard() { return m_Blackboard; }
		[[nodiscard]] const Blackboard& GetBlackboard() const { return m_Blackboard; }
		//const DependencyGraph& GetDependencyGraph() const { return m_DependencyGraph; }

		/* ���һ����Դ�ڵ��Ƿ���Ч */
		[[nodiscard]] bool IsHandleValid(FrameGraphResourceHandle handle) const;
		/* �ж�һ����Դ�����Ƿ���Ч */
		[[nodiscard]] bool IsConnectionValid(const SharedPtr<DependencyGraph::Connection>& connection) const;
		/* ���һ��FrameGraphPass�ڵ��Ƿ��Ż��� */
		[[nodiscard]] bool IsPassCulled(const SharedPtr<IFrameGraphPass>& pass) const;

		/* ����FrameGraph */
		void Build() noexcept;
		/* ִ�� */
		void Execute() noexcept;
		/* ���� */
		void Reset() noexcept;

		/* ��������ͼ http://dreampuf.github.io/GraphvizOnline/ */
		void ExportGraphviz(const std::string& path);

	private:
		/* �޳�����Ч��RenderPassNode */
		void CullRenderPassNodes();

		/* �����Դ��m_ResourcesMap */
		FrameGraphResourceHandle AddResourceInternal(const SharedPtr<IResource>& resource);

		/* ΪRenderPass��������Դ */
		void BindInputResourceInternal(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandle handle, 
			const std::function<bool(const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)>& callback = [](const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)->bool { return true; });

		/* ΪRenderPass�������Դ */
		void BindOutputResourceInternal(const SharedPtr<RenderPassNode>& render_pass_node, FrameGraphResourceHandle handle, 
			const std::function<bool(const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)>& callback = [](const SharedPtr<IResource>&, const SharedPtr<RenderResourceNode>&)->bool { return true; });

		/* �ͷ�FrameGraph */
		void Destroy();

		/* ����� */
		std::string m_DebugName{ "Unnamed FrameGraph" };
		/* RenderPassNode�б�������ǰFrameGraph����Pass�ڵ㣬��Cull�׶βŻ��޳�������Ч�Ľڵ� */
		std::vector<SharedPtr<RenderPassNode>> m_RenderPassNodes{};
		/* ��Դ�б�<IFrameGraphResourceHandle.m_Index, IResource> */
		std::unordered_map<uint16_t, SharedPtr<IResource>> m_ResourcesMap{};
		/* ��Դ��������Դ�ڵ�������ӳ��: first->IResource, second->RenderResourceNode */
		std::unordered_map<uint16_t, SharedPtr<RenderResourceNode>> m_ResourceToResourceNodeMap{};
		/* Blackboard: ��Դ������Դ��ӳ�� */
		Blackboard m_Blackboard{};
		/* Pass������ϵͼ */
		DependencyGraph m_DependencyGraph;
		/* ���һ����Ч��RenderPassNode */
		std::vector<SharedPtr<RenderPassNode>>::iterator m_LastValidRenderPassNodeIter;
	};

}
