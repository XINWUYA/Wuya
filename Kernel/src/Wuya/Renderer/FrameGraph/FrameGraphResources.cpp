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

	/* 获取当前RenderPassNode名 */
	const std::string& FrameGraphResources::GetPassName() const
	{
		return m_RenderPassNode.GetName();
	}

	/* 获取当前RenderPass的RenderTarget */
	const SharedPtr<FrameBuffer>& FrameGraphResources::GetPassRenderTarget(uint32_t idx) const
	{
		const auto render_pass_data = m_RenderPassNode.GetRenderPassData(idx);
		if (!render_pass_data)
		{
			CORE_LOG_ERROR("Failed to access the RenderTarget of RenderPass({}) by index({}).", m_RenderPassNode.GetName(), idx);
			return nullptr;
		}

		return render_pass_data->m_pRenderTarget;
	}

	/* 从FrameGraph中获取资源 */
	IResource* FrameGraphResources::GetResource(FrameGraphResourceHandle handle) const
	{
		ASSERT(handle.IsInitialized(), "Handle is invalid.");
		IResource* resource = m_FrameGraph.GetResource(handle);

		if (!m_RenderPassNode.IsResourceHandleRegistered(handle))
		{
			CORE_LOG_ERROR("RenderPassNode({}) has not register the resource({}).", m_RenderPassNode.GetName(), resource->GetName());
		}
		ASSERT(resource->GetRefCount(), "Resource is not needed by this RenderPassNode.");

		return resource;
	}

	/* 创建 */
	void FrameGraphTexture::Create(const std::string& name, const Descriptor& desc, Usage usage)
	{
		Texture = Texture::Create(name, { desc.Width, desc.Height, desc.Depth, desc.MipLevels, desc.Samples, desc.TextureFormat, desc.SamplerType, usage });
	}
}
