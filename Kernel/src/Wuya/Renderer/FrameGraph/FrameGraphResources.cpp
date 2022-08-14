#include "Pch.h"
#include "FrameGraphResources.h"
#include "Resource.h"
#include "RenderPassNode.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	FrameGraphResources::FrameGraphResources(FrameGraph& frame_graph, RenderPassNode& render_pass_node)
		: m_FrameGraph(frame_graph), m_RenderPassNode(render_pass_node)
	{
	}

	/* ��ȡ��ǰRenderPassNode�� */
	const std::string& FrameGraphResources::GetPassName() const
	{
		return m_RenderPassNode.GetDebugName();
	}

	/* ��ȡ��ǰRenderPass��RenderTarget */
	SharedPtr<FrameBuffer> FrameGraphResources::GetPassRenderTarget(uint32_t idx) const
	{
		const auto& render_pass_data = m_RenderPassNode.GetRenderPassData(idx);
		if (!render_pass_data)
		{
			CORE_LOG_ERROR("Failed to access the RenderTarget of RenderPass({}) by index({}).", m_RenderPassNode.GetDebugName(), idx);
			return nullptr;
		}

		return render_pass_data->FrameBuffer;
	}

	/* ��FrameGraph�л�ȡ��Դ */
	const SharedPtr<IResource>& FrameGraphResources::GetResource(FrameGraphResourceHandle handle) const
	{
		PROFILE_FUNCTION();

		ASSERT(handle.IsInitialized(), "Handle is invalid.");
		auto resource = m_FrameGraph.GetResource(handle);

		if (!m_RenderPassNode.IsResourceHandleRegistered(handle))
		{
			CORE_LOG_ERROR("RenderPassNode({}) has not register the resource({}).", m_RenderPassNode.GetDebugName(), resource->GetName());
		}
		ASSERT(resource->GetRefCount(), "Resource is not needed by this RenderPassNode.");

		return resource;
	}

	/* ���� */
	void FrameGraphTexture::Create(const std::string& name, const Descriptor& desc, Usage usage)
	{
		PROFILE_FUNCTION();

		Texture = Texture::Create(name, { desc.Width, desc.Height, desc.Depth, desc.MipLevels, desc.Samples, desc.TextureFormat, desc.SamplerType, usage });
	}

	/* ���� */
	void FrameGraphTexture::Destroy()
	{
		Texture = nullptr;
	}
}
