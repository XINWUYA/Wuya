#include "Pch.h"
#include "RenderPassNode.h"
#include "RenderResourceNode.h"
#include "Wuya/Renderer/FrameBuffer.h"

namespace Wuya
{
	RenderPassNode::RenderPassNode(FrameGraph& frame_graph, const std::string& name, const SharedPtr<IFrameGraphPass>& frame_graph_pass)
		: Node(frame_graph.GetDependencyGraph()), m_OwnerFrameGraph(frame_graph), m_OwnerFrameGraphPass(frame_graph_pass)
	{
		Name = name;
	}

	RenderPassNode::~RenderPassNode()
	{
		Destroy();
	}

	/* ע����ԴHandle */
	void RenderPassNode::RegisterResourceHandle(FrameGraphResourceHandle handle)
	{
		auto& resource = m_OwnerFrameGraph.GetResource(handle);
		resource->NeedByPass(shared_from_this());
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
		auto& attachments = data.Descriptor.Attachments;

		const auto& dependency_graph = m_OwnerFrameGraph.GetDependencyGraph();
		const auto incoming_connections = dependency_graph.GetIncomingConnectionsOfNode(shared_from_this());

		for (uint32_t i = 0; i < FrameGraphPassInfo::MAX_ATTACHMENT_NUM; ++i)
		{
			if (attachments.AttachmentArray[i])
			{
				data.ValidAttachments[i] = attachments.AttachmentArray[i];

				const auto target_resource_handle = desc.Attachments.AttachmentArray[i];

				/* �ж���Դ�ڵ��Ƿ�Ϊ����ڵ� */
				for (const auto& conn : incoming_connections)
				{
					auto resource_node = DynamicPtrCast<RenderResourceNode>(dependency_graph.GetNode(conn->FromNodeIdx));
					if (resource_node && (resource_node->GetResourceHandle() == target_resource_handle))
					{
						data.IncomingResourceNodes[i] = resource_node;
						break;
					}
				}

				/* Attached �ڵ��У���������ڵ㣬��Ϊ����ڵ� */
				data.OutgoingResourceNodes[i] = m_OwnerFrameGraph.GetRenderResourceNode(target_resource_handle);
				if (data.OutgoingResourceNodes[i] == data.IncomingResourceNodes[i])
					data.OutgoingResourceNodes[i] = nullptr;
			}
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
			/* ������Դ���� */
			for (uint32_t i = 0; i < FrameGraphPassInfo::MAX_ATTACHMENT_NUM; ++i)
			{
				if (render_pass_data.ValidAttachments[i])
					render_pass_data.RenderBufferUsage |= GetRenderBufferUsageByIndex(i);
			}


		}
	}

	/* ִ�н׶� */
	void RenderPassNode::Execute(const FrameGraphResources& resources)
	{
		/* ���ɵ�ǰ�ڵ����Դ���� */
		for (auto& render_pass_data : m_RenderPassDatas)
		{
			/* ����FrameBuffer���� */
			FrameBufferDesc desc;
			desc.ViewportRegion = { 0,0,1920, 1080 };
			desc.Samples = render_pass_data.Descriptor.Samples;
			desc.Usage = render_pass_data.RenderBufferUsage;

			/* ׼��Color Attachments */
			desc.ColorRenderBuffers.reserve(MAX_COLOR_ATTACHMENT_NUM);
			for (uint32_t i = 0; i < MAX_COLOR_ATTACHMENT_NUM; ++i)
			{
				if (render_pass_data.ValidAttachments[i])
				{
					const auto& fg_texture = DynamicPtrCast<const Resource<FrameGraphTexture>>(m_OwnerFrameGraph.GetResource(render_pass_data.ValidAttachments[i]));
					desc.ColorRenderBuffers.push_back({ fg_texture->GetResource().Texture, fg_texture->GetSubDescriptor().Level, fg_texture->GetSubDescriptor().Layer });
				}
			}

			/* ׼��Depth Attachment */
			if (render_pass_data.ValidAttachments[MAX_COLOR_ATTACHMENT_NUM])
			{
				const auto& fg_texture = DynamicPtrCast<const Resource<FrameGraphTexture>>(m_OwnerFrameGraph.GetResource(render_pass_data.ValidAttachments[MAX_COLOR_ATTACHMENT_NUM]));
				desc.DepthRenderBuffer.RenderTarget = fg_texture->GetResource().Texture;
				desc.DepthRenderBuffer.Level = fg_texture->GetSubDescriptor().Level;
				desc.DepthRenderBuffer.Layer = fg_texture->GetSubDescriptor().Layer;
			}

			/* ׼��Stencil Attachment */
			if (render_pass_data.ValidAttachments[MAX_COLOR_ATTACHMENT_NUM + 1])
			{
				const auto& fg_texture = DynamicPtrCast<const Resource<FrameGraphTexture>>(m_OwnerFrameGraph.GetResource(render_pass_data.ValidAttachments[MAX_COLOR_ATTACHMENT_NUM + 1]));
				desc.StencilRenderBuffer.RenderTarget = fg_texture->GetResource().Texture;
				desc.StencilRenderBuffer.Level = fg_texture->GetSubDescriptor().Level;
				desc.StencilRenderBuffer.Layer = fg_texture->GetSubDescriptor().Layer;
			}

			/* ����FrameBuffer */
			render_pass_data.FrameBuffer = FrameBuffer::Create(render_pass_data.Name + "_FrameBuffer", desc);
		}

		/* ִ�е�ǰPass */
		m_OwnerFrameGraphPass->Execute(resources);
	}

	/* ���� */
	void RenderPassNode::Destroy()
	{
		/* ���ٵ�ǰ�ڵ����Դ���� */
		for (auto& render_pass_data : m_RenderPassDatas)
		{
			/* ������Դ���� */
			render_pass_data.FrameBuffer.reset();
		}
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
