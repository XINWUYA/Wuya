#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Renderer/FrameBuffer.h"
#include <Metal/Metal.hpp>

namespace Helios
{
    class MetalFrameBuffer : public FrameBuffer
    {
    public:
        MetalFrameBuffer(const std::string& name, const FrameBufferDesc& desc);
        ~MetalFrameBuffer() override;

        void Bind() override;
        void Unbind() override;

        void Resize(uint32_t width, uint32_t height) override;

        void ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data) override;
        void ClearAttachment(uint32_t attachment_index, int level, const PixelDesc& pixel_desc, void* data) override;

        /* Metal特有接口 */
        MTL::RenderPassDescriptor* GetRenderPassDescriptor() const { return m_RenderPassDescriptor; }
        MTL::Texture* GetColorAttachment(uint32_t index) const;
        MTL::Texture* GetDepthAttachment() const;

    private:
        void CreateAttachments();
        void UpdateRenderPassDescriptor();

        MTL::RenderPassDescriptor* m_RenderPassDescriptor{ nullptr };
        std::vector<MTL::Texture*> m_ColorAttachments;
        MTL::Texture* m_DepthAttachment{ nullptr };
        MTL::Texture* m_StencilAttachment{ nullptr };
    };
}

#endif /* PLATFORM_MACOS */
