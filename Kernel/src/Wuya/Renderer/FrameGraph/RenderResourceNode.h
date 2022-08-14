#pragma once
#include "DependencyGraph.h"
#include "FrameGraphResourceHandle.h"

namespace Wuya
{
	class FrameGraph;
	class RenderPassNode;

	/* ��Ⱦ��Դ�ڵ��ࣺ
	 * ��������FrameGraph�ж�Ӧ��Handle��������ʵ����Դ���ݣ�ʵ����Դ������Resource����У���
	 * �ýڵ��д洢�뵱ǰ��Դ�ڵ���ص�����/������ߣ�
	 * Ŀǰһ����Դ�ڵ�ɶ�Ӧ���ʹ��Pass����Ϊ��ǰ��Դ�ڵ���������
	 * ����֧��һ��д��Pass����Ϊ��ǰ��Դ�ڵ�����룩��
	 */
	class RenderResourceNode : public DependencyGraph::Node
	{
	public:
		RenderResourceNode(const std::string& name, FrameGraph& frame_graph, FrameGraphResourceHandle handle);
		~RenderResourceNode() override;

		/* ��ȡ��ǰ��Դ��Handle */
		[[nodiscard]] const FrameGraphResourceHandle& GetResourceHandle() const { return m_ResourceHandle; }
		/* ��ȡ��Դ�� */
		[[nodiscard]] const std::string& GetDebugName() const { return DebugName; }

		/* ����ʹ�ýڵ� */
		void SetIncomingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_pIncomingConnection = connection; }
		void AddOutgoingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_OutgoingConnections.emplace_back(connection); }

		/* ��ȡ��Դ����Ϊ����/�����ʹ����� */
		[[nodiscard]] bool HasOutgoingConnection() const { return !m_OutgoingConnections.empty(); }
		[[nodiscard]] bool HasIncomingConnection() const { return m_pIncomingConnection != nullptr; }

		/* �ӽڵ�ĽǶȳ�������ȡ����Դ֮������� */
		[[nodiscard]] SharedPtr<DependencyGraph::Connection> GetOutgoingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;
		[[nodiscard]] SharedPtr<DependencyGraph::Connection> GetIncomingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;

		/* ������Դ��Usage */
		void UpdateResourceUsage();

		/* ���� */
		void Destroy();

	private:
		/* ������ӻ����� */
		[[nodiscard]] std::string Graphvizify() const override;
		[[nodiscard]] std::string GraphvizifyConnectionColor() const override;

		/* ��Դ�����һ�������ӦFrameGraph�е�һ����Դ */
		FrameGraphResourceHandle m_ResourceHandle;
		/* ������FrameGraph */
		FrameGraph& m_FrameGraph;
		/* ��Դ��ʹ�������һ����Դ���ܱ����Passʹ�ã������ֻ֧�ֱ�1��Pass��д�� */
		std::vector<SharedPtr<DependencyGraph::Connection>> m_OutgoingConnections{};
		SharedPtr<DependencyGraph::Connection> m_pIncomingConnection{ nullptr };
	};
}
