#pragma once

namespace Wuya
{
	/* 依赖关系图 */
	class DependencyGraph
	{
	public:
		/* 节点 */
		struct Node
		{
			const uint32_t Index; /* 节点索引，初始化之后就不再修改 */
			uint32_t RefCount{ 0 };
			std::string Name{ "unknown" };
			bool IsTarget{ false };

			Node(DependencyGraph& graph);
			Node(Node&&) = default;
			Node(const Node&) = delete;
			Node& operator=(const Node&) = delete;

			/* 是否为有效节点 */
			bool IsValid() const { return IsTarget ? true : RefCount > 0; }
			/* 是否被剔除 */
			bool IsCulled() const { return RefCount == 0; }

			/* 输出可视化文本 */
			virtual std::string Graphvizify() const;
			/* 指定当前节点发出的连线的可视化颜色 */
			virtual std::string GraphvizifyConnectionColor() const;
		};

		/* 连线 */
		struct Connection
		{
			/* 连接的两个节点 */
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

		/* 清楚所有节点和连线 */
		void Clear();
		/* 更新节点的引用计数 */
		void UpdateRefCount();

		/* 获取指定节点 */
		const Node* GetNode(uint32_t node_idx) const;
		Node* GetNode(uint32_t node_idx);
		/* 获取所有节点 */
		const std::vector<Node*>& GetNodes() const { return m_Nodes; }
		/* 获取所有连线 */
		const std::vector<Connection*>& GetConnections() const { return m_Connections; }
		/* 获取指定节点的所有入射连线 */
		std::vector<Connection*> GetIncomingConnectionsOfNode(const Node* node) const;
		/* 获取指定节点的所有出射连线 */
		std::vector<Connection*> GetOutgoingConnectionsOfNode(const Node* node) const;
		/* 判断连线是否有效 */
		bool IsValidConnection(const Connection* connection) const;

		/* 输出可视化文本：便于调试（将输出字符复制到：http://www.webgraphviz.com/） */
		std::string Graphvizify(const std::string& name = {}) const;

	private:
		/* 注册一个节点 */
		void RegisterNode(Node* node);
		/* 注册一个连线 */
		void RegisterConnection(Connection* connection);
		/* 生成节点索引 */
		uint32_t GenerateNodeIndex() const;

		/* 图中节点集合 */
		std::vector<Node*> m_Nodes;
		/* 图中的连线集合 */
		std::vector<Connection*> m_Connections;
	};
}
