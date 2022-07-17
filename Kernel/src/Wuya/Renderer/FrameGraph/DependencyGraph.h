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
			const uint32_t Index; /* �ڵ���������ʼ��֮��Ͳ����޸� */
			uint32_t RefCount{ 0 };
			std::string Name{ "unknown" };
			bool IsTarget{ false };

			Node(DependencyGraph& graph);
			Node(Node&&) = default;
			Node(const Node&) = delete;
			Node& operator=(const Node&) = delete;

			/* �Ƿ�Ϊ��Ч�ڵ� */
			bool IsValid() const { return IsTarget ? true : RefCount > 0; }
			/* �Ƿ��޳� */
			bool IsCulled() const { return RefCount == 0; }

			/* ������ӻ��ı� */
			virtual std::string Graphvizify() const;
			/* ָ����ǰ�ڵ㷢�������ߵĿ��ӻ���ɫ */
			virtual std::string GraphvizifyConnectionColor() const;
		};

		/* ���� */
		struct Connection
		{
			/* ���ӵ������ڵ� */
			uint32_t FromNodeIdx{ 0 };
			uint32_t ToNodeIdx{ 0 };

			Connection(DependencyGraph& graph, Node* from, Node* to);
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;
			bool operator==(const Connection& other) const
			{
				return other.FromNodeIdx == FromNodeIdx && other.ToNodeIdx == ToNodeIdx;
			}
		};

		DependencyGraph();
		virtual~DependencyGraph() = default;

		/* ������нڵ������ */
		void Clear();
		/* ���½ڵ�����ü��� */
		void UpdateRefCount();

		/* ��ȡָ���ڵ� */
		const Node* GetNode(uint32_t node_idx) const;
		Node* GetNode(uint32_t node_idx);
		/* ��ȡ���нڵ� */
		const std::vector<Node*>& GetNodes() const { return m_Nodes; }
		/* ��ȡ�������� */
		const std::vector<Connection*>& GetConnections() const { return m_Connections; }
		/* ��ȡָ���ڵ�������������� */
		std::vector<Connection*> GetIncomingConnectionsOfNode(const Node* node) const;
		/* ��ȡָ���ڵ�����г������� */
		std::vector<Connection*> GetOutgoingConnectionsOfNode(const Node* node) const;
		/* �ж������Ƿ���Ч */
		bool IsValidConnection(const Connection* connection) const;

		/* ������ӻ��ı������ڵ��ԣ�������ַ����Ƶ���http://www.webgraphviz.com/�� */
		std::string Graphvizify(const std::string& name = {}) const;

	private:
		/* ע��һ���ڵ� */
		void RegisterNode(Node* node);
		/* ע��һ������ */
		void RegisterConnection(Connection* connection);
		/* ���ɽڵ����� */
		uint32_t GenerateNodeIndex() const;

		/* ͼ�нڵ㼯�� */
		std::vector<Node*> m_Nodes;
		/* ͼ�е����߼��� */
		std::vector<Connection*> m_Connections;
	};
}
