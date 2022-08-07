#include "Pch.h"
#include "DependencyGraph.h"

#include <algorithm>

namespace Wuya
{
	DependencyGraph::Node::Node(DependencyGraph& graph)
		: Index(graph.GenerateNodeIndex())
	{
		PROFILE_FUNCTION();

		graph.RegisterNode(this);
	}

	std::string DependencyGraph::Node::Graphvizify() const
	{
		std::string result_str;
		result_str.reserve(128);
		result_str.append("[label=\"");
		result_str.append(Name);
		result_str.append("\\nrefs: ");
		result_str.append(std::to_string(RefCount));
		result_str.append(", id: ");
		result_str.append(std::to_string(Index));
		result_str.append("\", style=filled, fillcolor=");
		result_str.append(IsValid() ? "skyblue" : "skyblue4");
		result_str.append("]");
		result_str.shrink_to_fit();

		return  result_str;
	}

	std::string DependencyGraph::Node::GraphvizifyConnectionColor() const
	{
		return "darkolivegreen";
	}

	DependencyGraph::Connection::Connection(DependencyGraph& graph, Node* from, Node* to)
		: FromNodeIdx(from->Index), ToNodeIdx(to->Index)
	{
		PROFILE_FUNCTION();

		graph.RegisterConnection(this);
	}

	DependencyGraph::DependencyGraph()
	{
		PROFILE_FUNCTION();

		m_Nodes.reserve(8);
		m_Connections.reserve(16);
	}

	void DependencyGraph::Clear()
	{
		PROFILE_FUNCTION();

		m_Nodes.clear();
		m_Connections.clear();
	}

	void DependencyGraph::UpdateRefCount()
	{
		PROFILE_FUNCTION();

		/* 更新节点的引用计数 */
		for (const auto* connection : m_Connections)
		{
			GetNode(connection->FromNodeIdx)->RefCount++;
		}

		/* 筛选出无效的节点 */
		std::vector<Node*> invalid_nodes;
		std::copy_if(m_Nodes.begin(), m_Nodes.end(), std::back_inserter(invalid_nodes),
			[](const auto* node)->bool
			{
				return !node->IsValid();
			});

		/* 对于无效的节点，减少其入射节点的引用计数 */
		while (!invalid_nodes.empty())
		{
			const Node* node = invalid_nodes.back();
			invalid_nodes.pop_back();

			const auto incoming_connections = GetIncomingConnectionsOfNode(node);
			for(const auto* connection: incoming_connections)
			{
				Node* from_node = GetNode(connection->FromNodeIdx);
				from_node->RefCount--;

				if (!from_node->IsValid())
					invalid_nodes.emplace_back(from_node);
			}
		}
	}

	const DependencyGraph::Node* DependencyGraph::GetNode(uint32_t node_idx) const
	{
		PROFILE_FUNCTION();

		if (node_idx >= m_Nodes.size())
			return nullptr;

		return m_Nodes[node_idx];
	}

	DependencyGraph::Node* DependencyGraph::GetNode(uint32_t node_idx)
	{
		PROFILE_FUNCTION();

		if (node_idx >= m_Nodes.size())
			return nullptr;

		return m_Nodes[node_idx];
	}

	std::vector<DependencyGraph::Connection*> DependencyGraph::GetIncomingConnectionsOfNode(const Node* node) const
	{
		PROFILE_FUNCTION();

		std::vector<DependencyGraph::Connection*> result;

		const uint32_t target_idx = node->Index;
		std::copy_if(m_Connections.begin(), m_Connections.end(), std::back_inserter(result),
			[target_idx](const auto* connection)->bool
			{
				return connection->ToNodeIdx == target_idx;
			});

		return result;
	}

	std::vector<DependencyGraph::Connection*> DependencyGraph::GetOutgoingConnectionsOfNode(const Node* node) const
	{
		PROFILE_FUNCTION();

		std::vector<Connection*> result;

		const uint32_t target_idx = node->Index;
		std::copy_if(m_Connections.begin(), m_Connections.end(), std::back_inserter(result),
			[target_idx](const auto* connection)->bool
			{
				return connection->FromNodeIdx == target_idx;
			});

		return result;
	}

	bool DependencyGraph::IsConnectionValid(const Connection* connection) const
	{
		PROFILE_FUNCTION();

		const auto* from_node = GetNode(connection->FromNodeIdx);
		const auto* to_node = GetNode(connection->ToNodeIdx);
		return from_node->IsValid() && to_node->IsValid();
	}

	std::string DependencyGraph::Graphvizify(const std::string& name) const
	{
		std::ostringstream out_stream;

		const char* graph_name = name.empty() ? name.c_str() : "graph";
		out_stream << "digraph \"" << graph_name << "\" {\n";
		out_stream << "rankdir = LR\n";
		out_stream << "bgcolor = black\n";
		out_stream << "node [shape=rectangle, fontname=\"helvetica\", fontsize=10]\n\n";

		/* 各节点的内容 */
		for (const auto* node : m_Nodes) 
		{
			std::string s = node->Graphvizify();
			out_stream << "\"N" << node->Index << "\" " << s.c_str() << "\n";
		}

		/* 连线 */
		out_stream << "\n";
		for (const auto* node : m_Nodes) 
		{
			auto connections = GetOutgoingConnectionsOfNode(node);

			/* 重新排序，并筛选有效的连线 */
			auto iter_partition = std::partition(connections.begin(), connections.end(),
				[this](auto const& edge)
				{
					return IsConnectionValid(edge);
				});

			std::string s = node->GraphvizifyConnectionColor();

			auto iter_current = connections.begin();

			/* 先画有效的连线 */
			if (iter_current != iter_partition)
			{
				out_stream << "N" << node->Index << " -> { ";

				while (iter_current != iter_partition)
				{
					/*const auto* iter_ref = GetNode((*iter_current++)->ToNodeIdx);
					out_stream << "N" << iter_ref->Index << " ";*/
					out_stream << "N" << (*iter_current++)->ToNodeIdx << " ";
				}

				out_stream << "} [color=" << s.c_str() << "2]\n";
			}

			/* 再画无效的连线 */
			if (iter_current != connections.end()) 
			{
				out_stream << "N" << node->Index << " -> { ";

				while (iter_current != connections.end()) 
				{
					/*Node const* iter_next = GetNode((*iter_current++)->ToNodeIdx);
					out_stream << "N" << iter_next->Index << " ";*/
					out_stream << "N" << (*iter_current++)->ToNodeIdx << " "; 

				}
				out_stream << "} [color=" << s.c_str() << "4 style=dashed]\n";
			}
		}

		out_stream << "}" << std::endl;

		return out_stream.str();
	}

	void DependencyGraph::RegisterNode(Node* node)
	{
		PROFILE_FUNCTION();

		m_Nodes.emplace_back(node);
	}

	void DependencyGraph::RegisterConnection(Connection* connection)
	{
		PROFILE_FUNCTION();

		m_Connections.emplace_back(connection);
	}

	uint32_t DependencyGraph::GenerateNodeIndex() const
	{
		PROFILE_FUNCTION();

		return static_cast<uint32_t>(m_Nodes.size());
	}
}
