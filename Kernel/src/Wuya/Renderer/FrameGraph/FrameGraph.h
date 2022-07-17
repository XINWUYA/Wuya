#pragma once
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

	/* FrameGraph Builder��, ���ڽ�һ��RenderPass */
	class FrameGraphBuilder
	{
	public:
		explicit FrameGraphBuilder(FrameGraphBuilder&) = delete;
		FrameGraphBuilder& operator=(const FrameGraphBuilder&) = delete;

		/* ʹ��ǰ�ڵ㲻�ᱻ�Ż�������ʹû��ûʹ�õ� */
		void AsSideEffect(bool value = true);

		/* ������������һ��RenderPass */
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

		/* ��ȡ��Դ�� */
		const std::string& GetResourceName(FrameGraphResourceHandle handle) const;

		/* �������ֻ�ȡ��Դhandle */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> GetResourceHandle(const std::string& name) const
		{
			return m_FrameGraph.GetResourceHandleByName<ResourceType>(name);
		}

	private:
		FrameGraphBuilder(FrameGraph& frame_graph, RenderPassNode* render_pass_node);

		FrameGraph& m_FrameGraph;
		RenderPassNode* m_pRenderPassNode;

		friend class FrameGraph;
	};

	/* FrameGraph�� */
	class FrameGraph 
	{
	public:
		explicit FrameGraph();
		FrameGraph(FrameGraph&) = delete;
		FrameGraph& operator=(const FrameGraph&) = delete;
		~FrameGraph();

		/* ���һ����ִ�н׶ε�Pass */
		template<typename Data, typename SetupFunc>
		FrameGraphPass<Data>& AddPass(const std::string& name, SetupFunc setup_func)
		{
			auto* pass = new FrameGraphPass<Data>();
			RenderPassNode* node = new RenderPassNode(*this, name, pass);
			pass->SetRenderPassNode(node);
			m_RenderPassNodes.emplace_back(node);

			FrameGraphBuilder builder(*this, node);

			/* ִ��Setup�׶� */
			setup_func(builder, const_cast<Data&>(pass->GetData()));

			return *pass;
		}

		/* ���һ����ִ�н׶ε�Pass */
		template<typename Data, typename SetupFunc, typename ExecuteFunc>
		FrameGraphPass<Data>& AddPass(const std::string& name, SetupFunc setup_func, ExecuteFunc&& execute_func)
		{
			auto* pass = new FrameGraphPass_WithExecute<Data, ExecuteFunc>(std::forward<ExecuteFunc>(execute_func));
			RenderPassNode* node = new RenderPassNode(*this, name, pass);
			pass->SetRenderPassNode(node);
			m_RenderPassNodes.emplace_back(node);

			FrameGraphBuilder builder(*this, node);

			/* ִ��Setup�׶� */
			setup_func(builder, const_cast<Data&>(pass->GetData()));

			return *pass;
		}

		/* ������Դ */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> CreateResource(const std::string& name, const typename ResourceType::Descriptor& desc = {})
		{
			IResource* resource = new Resource<ResourceType>(name, desc);
			return FrameGraphResourceHandleTyped<ResourceType>(AddResourceInternal(resource));
		}

		/* ΪRenderPass��������Դ */
		template<typename ResourceType>
		void BindInputResource(RenderPassNode* render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultReadUsage)
		{
			BindInputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](IResource* resource, RenderResourceNode* resource_node)
				{
					/* ����Դ�ڵ����ӵ�����ͼ�� */
					auto res = static_cast<Resource<ResourceType>*>(resource);
					return res->ConnectAsInput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* ΪRenderPass�������Դ */
		template<typename ResourceType>
		void BindOutputResource(RenderPassNode* render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultWriteUsage)
		{
			BindOutputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](IResource* resource, RenderResourceNode* resource_node)
				{
					/* ����Դ�ڵ����ӵ�����ͼ�� */
					auto res = static_cast<Resource<ResourceType>*>(resource);
					return res->ConnectAsOutput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* �������ֻ�ȡ��Դhandle */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> GetResourceHandleByName(const std::string& name) const
		{
			return static_cast<FrameGraphResourceHandleTyped<ResourceType>>(GetResourceHandleByNameInternal(name));
		}

		/* ��ȡ��Դ */
		IResource* GetResource(FrameGraphResourceHandle handle);
		/* ��ȡ��Դ�ڵ� */
		RenderResourceNode* GetRenderResourceNode(FrameGraphResourceHandle handle);
		/* ��ȡ������ϵͼ */
		DependencyGraph& GetDependencyGraph() { return m_DependencyGraph; }
		//const DependencyGraph& GetDependencyGraph() const { return m_DependencyGraph; }

		/* ���һ����Դ�ڵ��Ƿ���Ч */
		bool IsHandleValid(FrameGraphResourceHandle handle) const;
		/* ���һ��FrameGraphPass�ڵ��Ƿ��Ż��� */
		bool IsPassCulled(IFrameGraphPass* pass) const;

		/* ����FrameGraph */
		void Build() noexcept;
		/* ִ�� */
		void Execute() noexcept;

		/* ��������ͼ http://dreampuf.github.io/GraphvizOnline/ */
		void ExportGraphviz(const std::string& path);

	private:
		/* �޳�����Ч��RenderPassNode */
		void CullRenderPassNodes();

		/* �����Դ��m_ResourcesMap */
		FrameGraphResourceHandle AddResourceInternal(IResource* resource);

		/* �������ֻ�ȡ��Դhandle */
		FrameGraphResourceHandle GetResourceHandleByNameInternal(const std::string& name) const;

		/* ΪRenderPass��������Դ */
		void BindInputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle, const std::function<bool(IResource*, RenderResourceNode*)>& callback = [](IResource*, RenderResourceNode*)->bool { return true; });

		/* ΪRenderPass�������Դ */
		void BindOutputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle, const std::function<bool(IResource*, RenderResourceNode*)>& callback = [](IResource*, RenderResourceNode*)->bool { return true; });

		/* �ͷ�FrameGraph */
		void Destroy();

		/* RenderPassNode�б�������ǰFrameGraph����Pass�ڵ㣬��Cull�׶βŻ��޳�������Ч�Ľڵ� */
		std::vector<RenderPassNode*> m_RenderPassNodes{};
		/* ��Դ�б�<IFrameGraphResourceHandle.m_Index, IResource> */
		std::unordered_map<uint16_t, IResource*> m_ResourcesMap{};
		/* ��Դ��������Դ�ڵ�������ӳ��: first->IResource, second->RenderResourceNode */
		std::unordered_map<uint16_t, RenderResourceNode*> m_ResourceToResourceNodeMap{};
		/* ��Դ������ԴHandle��ӳ��<name, FrameGraphResourceHandle> */
		std::unordered_map<std::string, FrameGraphResourceHandle> m_NameToResourceHandleMap{};

		/* Pass������ϵͼ */
		DependencyGraph m_DependencyGraph;
		/* ���һ����Ч��RenderPassNode */
		std::vector<RenderPassNode*>::iterator m_LastValidRenderPassNodeIter;
	};

}
