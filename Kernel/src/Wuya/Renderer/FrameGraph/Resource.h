#pragma once
#include "DependencyGraph.h"
#include "RenderResourceNode.h"
#include "RenderPassNode.h"

namespace Wuya
{
	//class RenderPassNode;

	/* ��Դ����� */
	class IResource
	{
	public:
		IResource(const std::string& name);
		IResource(const std::string& name, IResource* parent);
		IResource(const IResource&) = delete;
		IResource& operator=(const IResource&) = delete;
		virtual ~IResource() = default;

		/* ��Pass���� */
		void NeedByPass(RenderPassNode* pass_node);

		/* ��Դ�� */
		const std::string& GetName() const { return m_Name; }
		/* ��ȡ���ü��� */
		uint32_t GetRefCount() const { return m_RefCount; }
		/* ��ȡ��������Pass */
		RenderPassNode* GetFirstPassNode() const { return m_FirstPassNode; }
		RenderPassNode* GetLastPassNode() const { return m_LastPassNode; }

		/* ������Դ */
		virtual void Create() = 0;
		/* ������Դ */
		virtual void Destroy() = 0;
		/* �ͷ�һ������ */
		virtual void DestroyConnection(DependencyGraph::Connection* connection) = 0;
		/* ��ȡUsageStr */
		virtual std::string GetUsageStr() const = 0;

	protected:
		/* ��Դ��*/
		std::string m_Name;
		/* ����Դ�ڵ� */
		IResource* m_Parent{ nullptr };
		/* ��Դ���ü��� */
		uint32_t m_RefCount{ 0 };
		/* FrameGraph�е�һ��ʹ�ø���Դ��Pass */
		RenderPassNode* m_FirstPassNode{ nullptr };
		/* FrameGraph�����һ��ʹ�ø���Դ��Pass */
		RenderPassNode* m_LastPassNode{ nullptr };
	};

	/* ��Դ�� */
	template<typename ResourceType>
	class Resource : public IResource
	{
		using ResourceUsage = typename ResourceType::Usage;
	public:
		/* ��Դ�ڵ����� */
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

		/* ��ȡ��Դ���� */
		const ResourceType& GetResource() const { return m_Resource; }

		/* ������Դ���� */
		void SetDescriptor(const typename ResourceType::Descriptor& descriptor) { m_Descriptor = descriptor; }
		const typename ResourceType::Descriptor& GetDescriptor() const { return m_Descriptor; }

		/* ������Դ��; */
		void SetUsage(typename ResourceType::Usage usage) { m_Usage = usage; }
		typename ResourceType::Usage GetUsage() const { return m_Usage; }

		/* ����Դ�ڵ���Ϊ�������ӵ�����ͼ�� */
		bool ConnectAsInput(DependencyGraph& dependency_graph, RenderPassNode* render_pass_node, RenderResourceNode* resource_node, ResourceUsage usage)
		{
			/* ��ȡ��ǰRenderPass���������� */
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

		/* ����Դ�ڵ���Ϊ������ӵ�����ͼ�� */
		bool ConnectAsOutput(DependencyGraph& dependency_graph, RenderPassNode* render_pass_node, RenderResourceNode* resource_node, ResourceUsage usage)
		{
			/* ��ȡ��ǰRenderPass��������� */
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

		/* ������Դ */
		void Create() override
		{
			m_Resource.Create(m_Name, m_Descriptor, m_Usage);
		}

		/* ������Դ */
		void Destroy() override
		{
			
		}

		/* �ͷ�һ������ */
		void DestroyConnection(DependencyGraph::Connection* connection) override
		{
			delete static_cast<ResourceConnection*>(connection);
		}

		/* ��ȡUsageStr */
		std::string GetUsageStr() const override
		{
			return "";//std::to_string(m_Usage);
		}

	private:
		ResourceType m_Resource{}; /* ��Դ�������� */
		typename ResourceType::Descriptor m_Descriptor{}; /* ��Դ���� */
		typename ResourceType::Usage m_Usage; /* ��Դ��; */
	};
}
