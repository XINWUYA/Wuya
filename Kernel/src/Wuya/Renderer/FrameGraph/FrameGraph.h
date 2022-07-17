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

	/* FrameGraph Builder类, 用于将一个RenderPass */
	class FrameGraphBuilder
	{
	public:
		explicit FrameGraphBuilder(FrameGraphBuilder&) = delete;
		FrameGraphBuilder& operator=(const FrameGraphBuilder&) = delete;

		/* 使当前节点不会被优化掉，即使没有没使用到 */
		void AsSideEffect(bool value = true);

		/* 根据描述创建一个RenderPass */
		uint32_t CreateRenderPass(const std::string& name, const FrameGraphPassInfo::Descriptor& desc = {});
		/* 根据描述创建一个纹理资源 */
		FrameGraphResourceHandleTyped<FrameGraphTexture> CreateTexture(const std::string& name, const FrameGraphTexture::Descriptor& desc = {});

		/* 为当前RenderPass绑定输入资源 */
		template<typename ResourceType>
		void BindInputResource(FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultReadUsage)
		{
			m_FrameGraph.BindInputResource<ResourceType>(m_pRenderPassNode, handle, usage);
		}

		/* 为当前RenderPass绑定输出资源 */
		template<typename ResourceType>
		void BindOutputResource(FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultWriteUsage)
		{
			m_FrameGraph.BindOutputResource<ResourceType>(m_pRenderPassNode, handle, usage);
		}

		/* 获取资源名 */
		const std::string& GetResourceName(FrameGraphResourceHandle handle) const;

		/* 根据名字获取资源handle */
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

	/* FrameGraph类 */
	class FrameGraph 
	{
	public:
		explicit FrameGraph();
		FrameGraph(FrameGraph&) = delete;
		FrameGraph& operator=(const FrameGraph&) = delete;
		~FrameGraph();

		/* 添加一个无执行阶段的Pass */
		template<typename Data, typename SetupFunc>
		FrameGraphPass<Data>& AddPass(const std::string& name, SetupFunc setup_func)
		{
			auto* pass = new FrameGraphPass<Data>();
			RenderPassNode* node = new RenderPassNode(*this, name, pass);
			pass->SetRenderPassNode(node);
			m_RenderPassNodes.emplace_back(node);

			FrameGraphBuilder builder(*this, node);

			/* 执行Setup阶段 */
			setup_func(builder, const_cast<Data&>(pass->GetData()));

			return *pass;
		}

		/* 添加一个带执行阶段的Pass */
		template<typename Data, typename SetupFunc, typename ExecuteFunc>
		FrameGraphPass<Data>& AddPass(const std::string& name, SetupFunc setup_func, ExecuteFunc&& execute_func)
		{
			auto* pass = new FrameGraphPass_WithExecute<Data, ExecuteFunc>(std::forward<ExecuteFunc>(execute_func));
			RenderPassNode* node = new RenderPassNode(*this, name, pass);
			pass->SetRenderPassNode(node);
			m_RenderPassNodes.emplace_back(node);

			FrameGraphBuilder builder(*this, node);

			/* 执行Setup阶段 */
			setup_func(builder, const_cast<Data&>(pass->GetData()));

			return *pass;
		}

		/* 创建资源 */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> CreateResource(const std::string& name, const typename ResourceType::Descriptor& desc = {})
		{
			IResource* resource = new Resource<ResourceType>(name, desc);
			return FrameGraphResourceHandleTyped<ResourceType>(AddResourceInternal(resource));
		}

		/* 为RenderPass绑定输入资源 */
		template<typename ResourceType>
		void BindInputResource(RenderPassNode* render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultReadUsage)
		{
			BindInputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](IResource* resource, RenderResourceNode* resource_node)
				{
					/* 将资源节点连接到依赖图中 */
					auto res = static_cast<Resource<ResourceType>*>(resource);
					return res->ConnectAsInput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* 为RenderPass绑定输出资源 */
		template<typename ResourceType>
		void BindOutputResource(RenderPassNode* render_pass_node, FrameGraphResourceHandleTyped<ResourceType> handle, typename ResourceType::Usage usage = ResourceType::DefaultWriteUsage)
		{
			BindOutputResourceInternal(render_pass_node, handle,
				[this, render_pass_node, usage](IResource* resource, RenderResourceNode* resource_node)
				{
					/* 将资源节点连接到依赖图中 */
					auto res = static_cast<Resource<ResourceType>*>(resource);
					return res->ConnectAsOutput(m_DependencyGraph, render_pass_node, resource_node, usage);
				});
		}

		/* 根据名字获取资源handle */
		template<typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> GetResourceHandleByName(const std::string& name) const
		{
			return static_cast<FrameGraphResourceHandleTyped<ResourceType>>(GetResourceHandleByNameInternal(name));
		}

		/* 获取资源 */
		IResource* GetResource(FrameGraphResourceHandle handle);
		/* 获取资源节点 */
		RenderResourceNode* GetRenderResourceNode(FrameGraphResourceHandle handle);
		/* 获取依赖关系图 */
		DependencyGraph& GetDependencyGraph() { return m_DependencyGraph; }
		//const DependencyGraph& GetDependencyGraph() const { return m_DependencyGraph; }

		/* 检查一个资源节点是否有效 */
		bool IsHandleValid(FrameGraphResourceHandle handle) const;
		/* 检查一个FrameGraphPass节点是否被优化掉 */
		bool IsPassCulled(IFrameGraphPass* pass) const;

		/* 构建FrameGraph */
		void Build() noexcept;
		/* 执行 */
		void Execute() noexcept;

		/* 导出依赖图 http://dreampuf.github.io/GraphvizOnline/ */
		void ExportGraphviz(const std::string& path);

	private:
		/* 剔除掉无效的RenderPassNode */
		void CullRenderPassNodes();

		/* 添加资源到m_ResourcesMap */
		FrameGraphResourceHandle AddResourceInternal(IResource* resource);

		/* 根据名字获取资源handle */
		FrameGraphResourceHandle GetResourceHandleByNameInternal(const std::string& name) const;

		/* 为RenderPass绑定输入资源 */
		void BindInputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle, const std::function<bool(IResource*, RenderResourceNode*)>& callback = [](IResource*, RenderResourceNode*)->bool { return true; });

		/* 为RenderPass绑定输出资源 */
		void BindOutputResourceInternal(RenderPassNode* render_pass_node, FrameGraphResourceHandle handle, const std::function<bool(IResource*, RenderResourceNode*)>& callback = [](IResource*, RenderResourceNode*)->bool { return true; });

		/* 释放FrameGraph */
		void Destroy();

		/* RenderPassNode列表，包含当前FrameGraph所有Pass节点，再Cull阶段才会剔除其中无效的节点 */
		std::vector<RenderPassNode*> m_RenderPassNodes{};
		/* 资源列表<IFrameGraphResourceHandle.m_Index, IResource> */
		std::unordered_map<uint16_t, IResource*> m_ResourcesMap{};
		/* 资源索引与资源节点索引的映射: first->IResource, second->RenderResourceNode */
		std::unordered_map<uint16_t, RenderResourceNode*> m_ResourceToResourceNodeMap{};
		/* 资源名与资源Handle的映射<name, FrameGraphResourceHandle> */
		std::unordered_map<std::string, FrameGraphResourceHandle> m_NameToResourceHandleMap{};

		/* Pass依赖关系图 */
		DependencyGraph m_DependencyGraph;
		/* 最后一个有效的RenderPassNode */
		std::vector<RenderPassNode*>::iterator m_LastValidRenderPassNodeIter;
	};

}
