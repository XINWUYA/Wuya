#pragma once
#include "DependencyGraph.h"
#include "FrameGraphResourceHandle.h"

namespace Wuya
{
	class FrameGraph;
	class RenderPassNode;
	/* */
	class RenderResourceNode : public DependencyGraph::Node
	{
	public:
		RenderResourceNode(FrameGraph& frame_graph, FrameGraphResourceHandle handle);
		~RenderResourceNode() override;

		/* ��ȡ��ǰ��Դ��Handle */
		const FrameGraphResourceHandle& GetResourceHandle() const { return m_ResourceHandle; }
		/* ��ȡ��Դ�� */
		const std::string& GetName() const { return Name; }

		/* ����ʹ�ýڵ� */
		void SetIncomingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_pIncomingConnection = connection; }
		void AddOutgoingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_OutgoingConnections.emplace_back(connection); }
		/* ��ȡ��Դ����Ϊ����/�����ʹ����� */
		bool HasOutgoingConnection() const { return !m_OutgoingConnections.empty(); }
		bool HasIncomingConnection() const { return m_pIncomingConnection != nullptr; }
		/* �ӽڵ�ĽǶȳ�������ȡ����Դ֮������� */
		const SharedPtr<DependencyGraph::Connection>& GetOutgoingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;
		const SharedPtr<DependencyGraph::Connection>& GetIncomingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;
		/* ������Դ��Usage */
		void UpdateResourceUsage();

		/* ���� */
		void Destroy();

	private:
		/* ������ӻ����� */
		std::string Graphvizify() const override;
		std::string GraphvizifyConnectionColor() const override;

		/* ��Դ�����һ�������ӦFrameGraph�е�һ����Դ */
		FrameGraphResourceHandle m_ResourceHandle;
		/* ������FrameGraph */
		FrameGraph& m_FrameGraph;
		/* ��Դ��ʹ�������һ����Դ���ܱ����Passʹ�ã������ֻ֧�ֱ�1��Pass��д�� */
		std::vector<SharedPtr<DependencyGraph::Connection>> m_OutgoingConnections{};
		SharedPtr<DependencyGraph::Connection> m_pIncomingConnection{ nullptr };
	};
}
