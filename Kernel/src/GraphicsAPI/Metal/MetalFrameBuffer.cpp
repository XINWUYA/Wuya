#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalFrameBuffer.h"
#include "MetalCommon.h"
#include "MetalTexture.h"
#include "MetalRenderAPI.h"
#include "Helios/Renderer/Renderer.h"

namespace Helios
{
    MetalFrameBuffer::MetalFrameBuffer(const std::string& name, const FrameBufferDesc& desc)
        : DeviceFrameBuffer(name, desc)
    {
        /* 创建渲染通道描述符 */
        m_RenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        /* 创建附件 */
        CreateAttachments();

        /* 更新渲染通道描述符 */
        UpdateRenderPassDescriptor();

        CORE_LOG_INFO("Metal FrameBuffer created: {} ({}x{})", name, desc.ViewportRegion.Width, desc.ViewportRegion.Height);
    }

    MetalFrameBuffer::~MetalFrameBuffer()
    {
        /* 释放颜色附件 */
        for (auto texture : m_ColorAttachments)
        {
            if (texture)
                texture->release();
        }
        m_ColorAttachments.clear();

        /* 释放深度附件 */
        if (m_DepthAttachment)
            m_DepthAttachment->release();

        /* 释放模板附件 */
        if (m_StencilAttachment)
            m_StencilAttachment->release();

        /* 释放渲染通道描述符 */
        if (m_RenderPassDescriptor)
            m_RenderPassDescriptor->release();
    }

    void MetalFrameBuffer::Bind(const FrameBufferBindInfo& bind_info)
    {
        /* Metal中需要开始渲染通道来绑定帧缓冲区 */
        auto metalRenderAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        if (metalRenderAPI && m_RenderPassDescriptor)
        {
            metalRenderAPI->BeginRenderPass(m_RenderPassDescriptor);
        }
    }

    void MetalFrameBuffer::Unbind()
    {
        /* Metal中需要结束渲染通道来解绑帧缓冲区 */
        auto metalRenderAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        if (metalRenderAPI)
        {
            metalRenderAPI->EndRenderPass();
        }
    }

    void MetalFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > 8192 || height > 8192)
        {
            CORE_LOG_WARN("Attempted to resize framebuffer to {}, {}, but not supported!", width, height);
            return;
        }

        m_FrameBufferDesc.ViewportRegion.Width = width;
        m_FrameBufferDesc.ViewportRegion.Height = height;

        /* 重新创建附件 */
        for (auto texture : m_ColorAttachments)
        {
            if (texture)
                texture->release();
        }
        m_ColorAttachments.clear();

        if (m_DepthAttachment)
        {
            m_DepthAttachment->release();
            m_DepthAttachment = nullptr;
        }

        if (m_StencilAttachment)
        {
            m_StencilAttachment->release();
            m_StencilAttachment = nullptr;
        }

        CreateAttachments();
        UpdateRenderPassDescriptor();
    }

    void MetalFrameBuffer::ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data)
    {
        if (attachment_index >= m_ColorAttachments.size())
            return;

        auto texture = m_ColorAttachments[attachment_index];
        if (!texture)
            return;

        MTL::Region region;
        region.origin.x = x;
        region.origin.y = y;
        region.origin.z = 0;
        region.size.width = 1;
        region.size.height = 1;
        region.size.depth = 1;

        texture->getBytes(data, 4, region, 0);
    }

    void MetalFrameBuffer::ClearAttachment(uint32_t attachment_index, int level, const PixelDesc& pixel_desc, void* data)
    {
        /* Metal中清除操作在渲染通道开始时通过loadAction完成 */
    }

    MTL::Texture* MetalFrameBuffer::GetColorAttachment(uint32_t index) const
    {
        if (index < m_ColorAttachments.size())
            return m_ColorAttachments[index];
        return nullptr;
    }

    MTL::Texture* MetalFrameBuffer::GetDepthAttachment() const
    {
        return m_DepthAttachment;
    }

    void MetalFrameBuffer::CreateAttachments()
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
            return;

        uint32_t width = m_FrameBufferDesc.ViewportRegion.Width;
        uint32_t height = m_FrameBufferDesc.ViewportRegion.Height;

        /* 创建颜色附件 */
        for (const auto& render_buffer : m_FrameBufferDesc.ColorRenderBuffers)
        {
            if (!render_buffer.RenderTarget)
                continue;

            auto metal_texture = std::dynamic_pointer_cast<MetalTexture>(render_buffer.RenderTarget);
            if (metal_texture && metal_texture->GetMetalTexture())
            {
                m_ColorAttachments.push_back(metal_texture->GetMetalTexture());
                metal_texture->GetMetalTexture()->retain();
            }
            else
            {
                /* 创建新的颜色附件纹理 */
                MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
                textureDescriptor->setTextureType(m_FrameBufferDesc.Samples > 1 ? MTL::TextureType2DMultisample : MTL::TextureType2D);
                textureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
                textureDescriptor->setWidth(width);
                textureDescriptor->setHeight(height);
                textureDescriptor->setSampleCount(m_FrameBufferDesc.Samples);
                textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
                textureDescriptor->setStorageMode(MTL::StorageModePrivate);

                MTL::Texture* texture = device->newTexture(textureDescriptor);
                textureDescriptor->release();

                if (texture)
                    m_ColorAttachments.push_back(texture);
            }
        }

        /* 创建深度附件 */
        if (m_FrameBufferDesc.DepthRenderBuffer.RenderTarget)
        {
            auto metal_texture = std::dynamic_pointer_cast<MetalTexture>(m_FrameBufferDesc.DepthRenderBuffer.RenderTarget);
            if (metal_texture && metal_texture->GetMetalTexture())
            {
                m_DepthAttachment = metal_texture->GetMetalTexture();
                m_DepthAttachment->retain();
            }
        }
        else
        {
            /* 创建默认深度附件 */
            MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
            textureDescriptor->setTextureType(m_FrameBufferDesc.Samples > 1 ? MTL::TextureType2DMultisample : MTL::TextureType2D);
            textureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
            textureDescriptor->setWidth(width);
            textureDescriptor->setHeight(height);
            textureDescriptor->setSampleCount(m_FrameBufferDesc.Samples);
            textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
            textureDescriptor->setStorageMode(MTL::StorageModePrivate);

            m_DepthAttachment = device->newTexture(textureDescriptor);
            textureDescriptor->release();
        }
    }

    void MetalFrameBuffer::UpdateRenderPassDescriptor()
    {
        if (!m_RenderPassDescriptor)
            return;

        /* 配置颜色附件 */
        for (size_t i = 0; i < m_ColorAttachments.size(); ++i)
        {
            auto colorAttachment = m_RenderPassDescriptor->colorAttachments()->object(static_cast<NS::UInteger>(i));
            colorAttachment->setTexture(m_ColorAttachments[i]);
            colorAttachment->setLoadAction(MTL::LoadActionClear);
            colorAttachment->setStoreAction(MTL::StoreActionStore);
            colorAttachment->setClearColor(MTL::ClearColor(0.0, 0.0, 0.0, 1.0));
        }

        /* 配置深度附件 */
        if (m_DepthAttachment)
        {
            auto depthAttachment = m_RenderPassDescriptor->depthAttachment();
            depthAttachment->setTexture(m_DepthAttachment);
            depthAttachment->setLoadAction(MTL::LoadActionClear);
            depthAttachment->setStoreAction(MTL::StoreActionStore);
            depthAttachment->setClearDepth(1.0);
        }

        /* 配置模板附件 */
        if (m_StencilAttachment)
        {
            auto stencilAttachment = m_RenderPassDescriptor->stencilAttachment();
            stencilAttachment->setTexture(m_StencilAttachment);
            stencilAttachment->setLoadAction(MTL::LoadActionClear);
            stencilAttachment->setStoreAction(MTL::StoreActionStore);
            stencilAttachment->setClearStencil(0);
        }
    }
}

#endif /* PLATFORM_MACOS */
