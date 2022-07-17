#include "Pch.h"
#include "RenderPassNode.h"
#include "RenderResourceNode.h"

namespace Wuya
{
	RenderPassNode::RenderPassNode(FrameGraph& frame_graph, const std::string& name, IFrameGraphPass* frame_graph_pass)
		: Node(frame_graph.GetDependencyGraph()), m_OwnerFrameGraph(frame_graph), m_OwnerFrameGraphPass(frame_graph_pass)
	{
		Name = name;
	}

	/* ע����ԴHandle */
	void RenderPassNode::RegisterResourceHandle(FrameGraphResourceHandle handle)
	{
		IResource* resource = m_OwnerFrameGraph.GetResource(handle);
		resource->NeedByPass(this);
		m_RegisteredHandleIndices.insert(handle.GetIndex());
	}

	/* �ж���ԴHandle�Ƿ�ע�� */
	bool RenderPassNode::IsResourceHandleRegistered(FrameGraphResourceHandle handle) const
	{
		return m_RegisteredHandleIndices.find(handle.GetIndex()) != m_RegisteredHandleIndices.cend();
	}

	/* ����RenderPassData */
	uint32_t RenderPassNode::CreateRenderPassData(const std::string& name, const FrameGraphPassInfo::Descriptor& desc)
	{
		RenderPassData data;
		data.Name = name;
		data.Descriptor = desc;

		const auto& dependency_graph = m_OwnerFrameGraph.GetDependencyGraph();
		const auto incoming_connections = dependency_graph.GetIncomingConnectionsOfNode(this);

		for (uint32_t i = 0; i < FrameGraphPassInfo::MAX_ATTACHMENT_NUM; ++i)
		{
			const auto target_resource_handle = desc.Attachments.AttachmentArray[i];

			/* �ж���Դ�ڵ��Ƿ�Ϊ����ڵ� */
			for (const auto& conn : incoming_connections)
			{
				RenderResourceNode* resource_node = (RenderResourceNode*)(dependency_graph.GetNode(conn->FromNodeIdx));
				if (resource_node->GetResourceHandle() == target_resource_handle)
				{
					data.IncomingResourceNodes[i] = resource_node;
					break;
				}
			}

			/* attached �ڵ��У���������ڵ㣬��Ϊ����ڵ� */
			data.OutgoingResourceNodes[i] = m_OwnerFrameGraph.GetRenderResourceNode(target_resource_handle);
			if (data.OutgoingResourceNodes[i] == data.IncomingResourceNodes[i])
				data.OutgoingResourceNodes[i] = nullptr;
		}

		uint32_t idx =  m_RenderPassDatas.size();
		m_RenderPassDatas.emplace_back(data);
		return idx;
	}

	/* ��ȡRenderPassData */
	const RenderPassData* RenderPassNode::GetRenderPassData(uint32_t idx) const
	{
		if (idx >= m_RenderPassDatas.size())
			return nullptr;
		return &m_RenderPassDatas[idx];
	}

	/* �����׶� */
	void RenderPassNode::Build()
	{
		for (auto& render_pass_data : m_RenderPassDatas)
		{
			
		}
	}

	/* ִ�н׶� */
	void RenderPassNode::Execute(const FrameGraphResources& resources)
	{
		/* ���ɵ�ǰ�ڵ����Դ���� */
		for (auto& data : m_RenderPassDatas)
		{
			/* ������Դ���� */
		}

		/* ִ�е�ǰPass */
		m_OwnerFrameGraphPass->Execute(resources);

		/* ���ٵ�ǰ�ڵ����Դ���� */
		for (auto& data : m_RenderPassDatas)
		{
			/* ������Դ���� */
		}
	}

	/* ���� */
	void RenderPassNode::Destroy()
	{
	}

	/* ������ӻ��ı� */
	std::string RenderPassNode::Graphvizify() const
	{
		std::string result_str;
		result_str.reserve(128);
		result_str.append("[label=\"");
		result_str.append(Name);
		result_str.append("\\nrefs: ");
		result_str.append(std::to_string(RefCount));
		result_str.append(", id: ");
		result_str.append(std::to_string(Index));
		for (auto& render_pass_data: m_RenderPassDatas)
		{
			result_str.append("\\ndata: ");
			result_str.append(render_pass_data.Name);
		}
		result_str.append("\", style=filled, fillcolor=");
		result_str.append(IsValid() ? "darkorange" : "darkorange4");
		result_str.append("]");
		result_str.shrink_to_fit();

		return  result_str;
	}

	/* ָ����ǰ�ڵ㷢�������ߵĿ��ӻ���ɫ */
	std::string RenderPassNode::GraphvizifyConnectionColor() const
	{
		return "red";
	}
}
