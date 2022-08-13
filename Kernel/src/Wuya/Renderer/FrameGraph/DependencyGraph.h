#pragma once

namespace Wuya
{
	/* ������ϵͼ */
	class DependencyGraph
	{
	public:
		/* �ڵ� */
		struct Node
		{
			std::string DebugName{ "Unnamed Node" };	/* ����� */
			const uint32_t Index;							/* �ڵ���������ʼ��֮��Ͳ����޸� */
			uint32_t RefCount{ 0 };							/* ��ǰ�ڵ㱻���õĴ��� */
			bool IsTarget{ false };							/* �Ƿ���ΪĿ��ڵ� */
			DependencyGraph& m_OwnerGraph;					/* ����Dependency */

			Node(std::string name, DependencyGraph& graph);
			Node(Node&&) = default;
			virtual ~Node() = default;
			Node(const Node&) = delete;
			Node& operator=(const Node&) = delete;

			/* �Ƿ�Ϊ��Ч�ڵ� */
			[[nodiscard]] bool IsValid() const { return IsTarget ? true : RefCount > 0; }
			/* �Ƿ��޳� */
			[[nodiscard]] bool IsCulled() const { return RefCount == 0; }

			/* ������ӻ��ı� */
			[[nodiscard]] virtual std::string Graphvizify() const;
			/* ָ����ǰ�ڵ㷢�������ߵĿ��ӻ���ɫ */
			[[nodiscard]] virtual std::string GraphvizifyConnectionColor() const;
		};

		/* ���� */
		struct Connection
		{
			uint32_t FromNodeIdx{ 0 };						/* ���ߵ����ڵ� */
			uint32_t ToNodeIdx{ 0 };						/* ���ߵ��յ�ڵ� */

			Connection(const SharedPtr<Node>& from, const SharedPtr<Node>& to);
			virtual ~Connection() = default;
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;
			/* �ж�������ͬ */
			bool operator==(const Connection& other) const
			{
				return other.FromNodeIdx == FromNodeIdx && other.ToNodeIdx == ToNodeIdx;
			}
		};

		DependencyGraph();
		virtual~DependencyGraph() = default;

		/* ע��һ���ڵ㵽����ͼ */
		void RegisterNode(const SharedPtr<Node>& node);
		/* ע��һ�����ߵ�����ͼ */
		void RegisterConnection(const SharedPtr<Connection>& connection);

		/* ������нڵ������ */
		void Clear();
		/* ���½ڵ�����ü��� */
		void UpdateRefCount();

		/* ��ȡָ���ڵ� */
		[[nodiscard]] SharedPtr<Node> GetNode(uint32_t node_idx) const;
		/* ��ȡ���нڵ� */
		[[nodiscard]] const std::vector<SharedPtr<Node>>& GetNodes() const { return m_Nodes; }
		/* ��ȡ�������� */
		[[nodiscard]] const std::vector<SharedPtr<Connection>>& GetConnections() const { return m_Connections; }
		/* ��ȡָ���ڵ�������������� */
		[[nodiscard]] std::vector<SharedPtr<Connection>> GetIncomingConnectionsOfNode(const SharedPtr<Node>& node) const;
		/* ��ȡָ���ڵ�����г������� */
		[[nodiscard]] std::vector<SharedPtr<Connection>> GetOutgoingConnectionsOfNode(const SharedPtr<Node>& node) const;
		/* �ж������Ƿ���Ч */
		[[nodiscard]] bool IsConnectionValid(const SharedPtr<Connection>& connection) const;

		/* ������ӻ��ı������ڵ��ԣ�������ַ����Ƶ���http://www.webgraphviz.com/�� */
		[[nodiscard]] std::string Graphvizify(const std::string& name = {}) const;

	private:
		/* ���ɽڵ����� */
		[[nodiscard]] uint32_t GenerateNodeIndex() const;

		/* ͼ�нڵ㼯�� */
		std::vector<SharedPtr<Node>> m_Nodes;
		/* ͼ�е����߼��� */
		std::vector<SharedPtr<Connection>> m_Connections;
	};
}
