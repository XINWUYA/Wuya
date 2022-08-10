#include "Pch.h"
#include "RenderResourceNode.h"
#include "FrameGraph.h"
#include "Resource.h"
#include "RenderPassNode.h"

namespace Wuya
{
	RenderResourceNode::RenderResourceNode(FrameGraph& frame_graph, FrameGraphResourceHandle handle)
		: Node(frame_graph.GetDependencyGraph()), m_ResourceHandle(handle), m_FrameGraph(frame_graph)
	{
		Name = m_FrameGraph.GetResource(m_ResourceHandle)->GetName();
	}

	RenderResourceNode::~RenderResourceNode()
	{
		Destroy();
	}

	const SharedPtr<DependencyGraph::Connection>& RenderResourceNode::GetOutgoingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const
	{
		/* �ж���Դ�����������Ƿ����Ը�PassNode */
		return m_pIncomingConnection && (m_pIncomingConnection->FromNodeIdx == node->Index) ? m_pIncomingConnection : nullptr;
	}

	const SharedPtr<DependencyGraph::Connection>& RenderResourceNode::GetIncomingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const
	{
		/* ������Դ��������ߣ��ж��Ƿ���е�ָ��PassNode������ */
		const auto find_pos = std::find_if(m_OutgoingConnections.begin(), m_OutgoingConnections.end(),
			[node](const SharedPtr<DependencyGraph::Connection>& connection)->bool
			{
				return connection->ToNodeIdx == node->Index;
			});

		return find_pos == m_OutgoingConnections.end() ? nullptr : *find_pos;
	}

	/* ������Դ��Usage */
	void RenderResourceNode::UpdateResourceUsage()
	{
		auto& resource = m_FrameGraph.GetResource(m_ResourceHandle);
		if (resource && resource->GetRefCount())
		{
			/* ��Դ��Ϊ�����Usage */
			for (auto& outging_connection : m_OutgoingConnections)
			{
				if (m_FrameGraph.IsConnectionValid(outging_connection))
				{
					resource->UpdateUsage(outging_connection);
				}
			}

			/* ��Դ��Ϊ�����Usage */
			if (m_pIncomingConnection)
			{
				resource->UpdateUsage(m_pIncomingConnection);
			}
		}
	}

	void RenderResourceNode::Destroy()
	{
		/* ��ͨ��Resource���ͷ� */
		auto& resource = m_FrameGraph.GetResource(m_ResourceHandle);

		resource->DestroyConnection(m_pIncomingConnection);
		m_pIncomingConnection = nullptr;

		for (auto& conn : m_OutgoingConnections)
			resource->DestroyConnection(conn);
		m_OutgoingConnections.clear();
	}

	/* ������ӻ����� */
	std::string RenderResourceNode::Graphvizify() const
	{
		auto& resource = m_FrameGraph.GetResource(m_ResourceHandle);

		std::string result_str;
		result_str.reserve(128);

		result_str.append("[label=\"");
		result_str.append(Name);
		result_str.append("\\nrefs: ");
		result_str.append(std::to_string(RefCount));
		result_str.append(", id: ");
		result_str.append(std::to_string(Index));
		result_str.append("\\nusage: ");
		result_str.append(resource->GetUsageStr());
		result_str.append("\", style=filled, fillcolor=");
		result_str.append(IsValid() ? "skyblue" : "skyblue4");
		result_str.append("]");
		result_str.shrink_to_fit();

		return  result_str;
	}

	std::string RenderResourceNode::GraphvizifyConnectionColor() const
	{
		return "darkolivegreen";
	}
}
