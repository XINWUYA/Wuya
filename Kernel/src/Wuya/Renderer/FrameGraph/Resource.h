#pragma once
#include "DependencyGraph.h"
#include "RenderResourceNode.h"
#include "RenderPassNode.h"

namespace Wuya
{
	//class RenderPassNode;

	/* 资源类基类 */
	class IResource
	{
	public:
		IResource(const std::string& name);
		IResource(const std::string& name, IResource* parent);
		IResource(const IResource&) = delete;
		IResource& operator=(const IResource&) = delete;
		virtual ~IResource() = default;

		/* 被Pass依赖 */
		void NeedByPass(RenderPassNode* pass_node);

		/* 资源名 */
		const std::string& GetName() const { return m_Name; }
		/* 获取引用计数 */
		uint32_t GetRefCount() const { return m_RefCount; }
		/* 获取被依赖的Pass */
		RenderPassNode* GetFirstPassNode() const { return m_FirstPassNode; }
		RenderPassNode* GetLastPassNode() const { return m_LastPassNode; }

		/* 创建资源 */
		virtual void Create() = 0;
		/* 销毁资源 */
		virtual void Destroy() = 0;
		/* 释放一条连线 */
		virtual void DestroyConnection(DependencyGraph::Connection* connection) = 0;
		/* 获取UsageStr */
		virtual std::string GetUsageStr() const = 0;

	protected:
		/* 资源名*/
		std::string m_Name;
		/* 父资源节点 */
		IResource* m_Parent{ nullptr };
		/* 资源引用计数 */
		uint32_t m_RefCount{ 0 };
		/* FrameGraph中第一次使用该资源的Pass */
		RenderPassNode* m_FirstPassNode{ nullptr };
		/* FrameGraph中最后一次使用该资源的Pass */
		RenderPassNode* m_LastPassNode{ nullptr };
	};

	/* 资源类 */
	template<typename ResourceType>
	class Resource : public IResource
	{
		using ResourceUsage = typename ResourceType::Usage;
	public:
		/* 资源节点连线 */
		struct ResourceConnection : public DependencyGraph::Connection
		{
			ResourceUsage Usg;

			ResourceConnection(DependencyGraph& graph, DependencyGraph::Node* from, DependencyGraph::Node* to, ResourceUsage usage)
				: DependencyGraph::Connection(graph, from, to), Usg(usage)
			{}
		};

		explicit Resource(const std::string& name, const typename ResourceType::Descriptor& desc)
			: IResource(name), m_Descriptor(desc)
		{
		}
		Resource(const std::string& name, IResource* parent)
			: IResource(name, parent)
		{
		}
		explicit Resource(const Resource&) = delete;

		/* 获取资源本体 */
		const ResourceType& GetResource() const { return m_Resource; }

		/* 设置资源描述 */
		void SetDescriptor(const typename ResourceType::Descriptor& descriptor) { m_Descriptor = descriptor; }
		const typename ResourceType::Descriptor& GetDescriptor() const { return m_Descriptor; }

		/* 设置资源用途 */
		void SetUsage(typename ResourceType::Usage usage) { m_Usage = usage; }
		typename ResourceType::Usage GetUsage() const { return m_Usage; }

		/* 将资源节点作为输入连接到依赖图中 */
		bool ConnectAsInput(DependencyGraph& dependency_graph, RenderPassNode* render_pass_node, RenderResourceNode* resource_node, ResourceUsage usage)
		{
			/* 获取当前RenderPass的输入连线 */
			ResourceConnection* connection = static_cast<ResourceConnection*>(resource_node->GetIncomingConnectionOfPassNode(render_pass_node));
			if (connection)
			{
				connection->Usg = ResourceUsage((uint8_t)connection->Usg | (uint8_t)usage);
			}
			else
			{
				connection = new ResourceConnection(dependency_graph, resource_node, render_pass_node, usage);
				resource_node->AddOutgoingConnection(connection);
			}
			return true;
		}

		/* 将资源节点作为输出连接到依赖图中 */
		bool ConnectAsOutput(DependencyGraph& dependency_graph, RenderPassNode* render_pass_node, RenderResourceNode* resource_node, ResourceUsage usage)
		{
			/* 获取当前RenderPass的输出连线 */
			ResourceConnection* connection = static_cast<ResourceConnection*>(resource_node->GetOutgoingConnectionOfPassNode(render_pass_node));
			if (connection)
			{
				connection->Usg = ResourceUsage(std::underlying_type_t<ResourceUsage>(connection->Usg) | std::underlying_type_t<ResourceUsage>(usage));
			}
			else
			{
				connection = new ResourceConnection(dependency_graph, render_pass_node, resource_node, usage);
				resource_node->SetIncomingConnection(connection);
			}
			return true;
		}

		/* 创建资源 */
		void Create() override
		{
			m_Resource.Create(m_Name, m_Descriptor, m_Usage);
		}

		/* 销毁资源 */
		void Destroy() override
		{
			
		}

		/* 释放一条连线 */
		void DestroyConnection(DependencyGraph::Connection* connection) override
		{
			delete static_cast<ResourceConnection*>(connection);
		}

		/* 获取UsageStr */
		std::string GetUsageStr() const override
		{
			return "";//std::to_string(m_Usage);
		}

	private:
		ResourceType m_Resource{}; /* 资源本体数据 */
		typename ResourceType::Descriptor m_Descriptor{}; /* 资源描述 */
		typename ResourceType::Usage m_Usage; /* 资源用途 */
	};
}
