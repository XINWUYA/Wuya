#pragma once
#include "DependencyGraph.h"
#include "RenderResourceNode.h"

namespace Wuya
{
	class RenderPassNode;

	/* ��Դ����� */
	class IResource
	{
	public:
		IResource(std::string name);
		IResource(std::string name, const SharedPtr<IResource>& parent);
		IResource(const IResource&) = delete;
		IResource& operator=(const IResource&) = delete;
		virtual ~IResource() = default;

		/* ��Pass���� */
		void NeedByPass(const SharedPtr<RenderPassNode>& pass_node);

		/* ��Դ�� */
		const std::string& GetName() const { return m_Name; }
		/* ��ȡ���ü��� */
		uint32_t GetRefCount() const { return m_RefCount; }
		/* ��ȡ��������Pass */
		const SharedPtr<RenderPassNode>& GetFirstPassNode() const { return m_FirstPassNode; }
		const SharedPtr<RenderPassNode>& GetLastPassNode() const { return m_LastPassNode; }

		/* ������Դ */
		virtual void Create() = 0;
		/* ������Դ */
		virtual void Destroy() = 0;
		/* �ͷ�һ������ */
		virtual void DestroyConnection(const SharedPtr<DependencyGraph::Connection>& connection) = 0;
		/* ��ȡUsageStr */
		virtual std::string GetUsageStr() const = 0;
		/* ������Դ��Usage */
		virtual void UpdateUsage(const SharedPtr<DependencyGraph::Connection>& connection) = 0;

	protected:
		/* ��Դ��*/
		std::string m_Name;
		/* ����Դ�ڵ� */
		SharedPtr<IResource> m_Parent{ nullptr };
		/* ��Դ���ü��� */
		uint32_t m_RefCount{ 0 };
		/* FrameGraph�е�һ��ʹ�ø���Դ��Pass */
		SharedPtr<RenderPassNode> m_FirstPassNode{ nullptr };
		/* FrameGraph�����һ��ʹ�ø���Դ��Pass */
		SharedPtr<RenderPassNode> m_LastPassNode{ nullptr };
	};

	/* ��Դ�� */
	template<typename ResourceType>
	class Resource : public IResource
	{
	public:
		using ResourceUsage = typename ResourceType::Usage;
		using ResourceDescriptor = typename ResourceType::Descriptor;
		using ResourceSubDescriptor = typename ResourceType::SubDescriptor;

		/* ��Դ�ڵ����� */
		struct ResourceConnection : public DependencyGraph::Connection
		{
			ResourceUsage Usage;

			ResourceConnection(const SharedPtr<DependencyGraph::Node>& from, const SharedPtr<DependencyGraph::Node>& to, ResourceUsage usage)
				: DependencyGraph::Connection(from, to), Usage(usage)
			{
			}

			~ResourceConnection() override = default;
		};

		Resource(const std::string& name, const ResourceDescriptor& desc)
			: IResource(name), m_Descriptor(desc)
		{
		}
		Resource(const std::string& name, const SharedPtr<IResource>& parent, const ResourceSubDescriptor& sub_desc)
			: IResource(name, parent), m_SubDescriptor(sub_desc)
		{
		}
		explicit Resource(const Resource&) = delete;

		/* ��ȡ��Դ���� */
		const ResourceType& GetResource() const { return m_Resource; }

		/* ������Դ���� */
		void SetDescriptor(const ResourceDescriptor& descriptor) { m_Descriptor = descriptor; }
		const ResourceDescriptor& GetDescriptor() const { return m_Descriptor; }

		/* ��������Դ���� */
		void SetSubDescriptor(const ResourceSubDescriptor& sub_descriptor) { m_SubDescriptor = sub_descriptor; }
		const ResourceSubDescriptor& GetSubDescriptor() const { return m_SubDescriptor; }

		/* ������Դ��; */
		void SetUsage(ResourceUsage usage) { m_Usage = usage; }
		ResourceUsage GetUsage() const { return m_Usage; }

		/* ����Դ�ڵ���Ϊ�������ӵ�����ͼ�� */
		bool ConnectAsInput(DependencyGraph& dependency_graph, const SharedPtr<RenderPassNode>& render_pass_node, const SharedPtr<RenderResourceNode>& resource_node, ResourceUsage usage)
		{
			/* ��ȡ��ǰRenderPass���������� */
			auto connection = StaticPtrCast<ResourceConnection>(resource_node->GetIncomingConnectionOfPassNode(render_pass_node));
			if (connection)
			{
				connection->Usage |= usage;
			}
			else
			{
				connection = CreateSharedPtr<ResourceConnection>(resource_node, render_pass_node, usage);
				/* ������ע�ᵽ����ͼ�� */
				dependency_graph.RegisterConnection(connection);
				/* Ҳ�����߱��浽��ǰ��Դ�ڵ� */
				resource_node->AddOutgoingConnection(connection);
			}
			return true;
		}

		/* ����Դ�ڵ���Ϊ������ӵ�����ͼ�� */
		bool ConnectAsOutput(DependencyGraph& dependency_graph, const SharedPtr<RenderPassNode>& render_pass_node, const SharedPtr<RenderResourceNode>& resource_node, ResourceUsage usage)
		{
			/* ��ȡ��ǰRenderPass��������� */
			auto connection = StaticPtrCast<ResourceConnection>(resource_node->GetOutgoingConnectionOfPassNode(render_pass_node));
			if (connection)
			{
				connection->Usage |= usage;
			}
			else
			{
				connection = CreateSharedPtr<ResourceConnection>(render_pass_node, resource_node, usage);
				/* ������ע�ᵽ����ͼ�� */
				dependency_graph.RegisterConnection(connection);
				/* Ҳ�����߱��浽��ǰ��Դ�ڵ� */
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
			m_Resource.Destroy();
		}

		/* �ͷ�һ������ */
		void DestroyConnection(const SharedPtr<DependencyGraph::Connection>& connection) override
		{
			StaticPtrCast<ResourceConnection>(connection).reset();
		}

		/* ��ȡUsageStr */
		std::string GetUsageStr() const override
		{
			return "";//std::to_string(m_Usage);
		}

		/* ������Դ��Usage */
		void UpdateUsage(const SharedPtr<DependencyGraph::Connection>& connection) override
		{
			auto conn = StaticPtrCast<ResourceConnection>(connection);
			m_Usage |= conn->Usage;
		}

	private:
		ResourceType m_Resource{}; /* ��Դ�������� */
		ResourceDescriptor m_Descriptor{}; /* ��Դ���� */
		ResourceSubDescriptor m_SubDescriptor{}; /* ����Դ���� */
		ResourceUsage m_Usage{}; /* ��Դ��; */
	};
}
