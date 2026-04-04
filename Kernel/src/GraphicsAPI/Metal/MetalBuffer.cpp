#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalBuffer.h"
#include "MetalCommon.h"
#include "MetalRenderAPI.h"
#include "Wuya/Renderer/Renderer.h"

namespace Wuya
{
    /* MetalVertexBuffer实现 */
    MetalVertexBuffer::MetalVertexBuffer(uint32_t size)
        : m_Size(size)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        m_Buffer = device->newBuffer(size, MTL::ResourceOptionCPUCacheModeDefault);
        if (!m_Buffer)
        {
            CORE_LOG_ERROR("Failed to create Metal vertex buffer!");
        }
    }

    MetalVertexBuffer::MetalVertexBuffer(const void* vertices, uint32_t size)
        : m_Size(size)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        m_Buffer = device->newBuffer(vertices, size, MTL::ResourceOptionCPUCacheModeDefault);
        if (!m_Buffer)
        {
            CORE_LOG_ERROR("Failed to create Metal vertex buffer!");
        }
    }

    MetalVertexBuffer::~MetalVertexBuffer()
    {
        if (m_Buffer)
            m_Buffer->release();
    }

    void MetalVertexBuffer::Bind() const
    {
        /* Metal中顶点缓冲区在绘制时绑定 */
    }

    void MetalVertexBuffer::Unbind() const
    {
        /* Metal中无需解绑 */
    }

    void MetalVertexBuffer::SetData(const void* data, uint32_t size)
    {
        if (!m_Buffer || size > m_Size)
        {
            CORE_LOG_ERROR("Invalid buffer or size!");
            return;
        }

        void* buffer_data = m_Buffer->contents();
        if (buffer_data)
        {
            memcpy(buffer_data, data, size);
            /* 对于共享存储模式的缓冲区，不需要调用didModifyRange */
            /* 数据会自动同步到GPU */
            //m_Buffer->didModifyRange({ 0, size });
        }
    }

    void MetalVertexBuffer::SetLayout(const VertexBufferLayout& layout)
    {
        m_Layout = layout;
    }

    uint32_t MetalVertexBuffer::GetVertexCount() const
    {
        if (m_Layout.GetStride() == 0)
            return 0;
        return m_Size / m_Layout.GetStride();
    }

    /* MetalIndexBuffer实现 */
    /* MetalIndexBuffer实现 - uint16版本 */
    MetalIndexBuffer::MetalIndexBuffer(const uint16_t* indices, uint32_t count)
        : m_Count(count), m_IndexType(IndexType::UInt16)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        m_Buffer = device->newBuffer(indices, count * sizeof(uint16_t), MTL::ResourceOptionCPUCacheModeDefault);
        if (!m_Buffer)
        {
            CORE_LOG_ERROR("Failed to create Metal index buffer (uint16)!");
        }
    }

    /* MetalIndexBuffer实现 - uint32版本 */
    MetalIndexBuffer::MetalIndexBuffer(const uint32_t* indices, uint32_t count)
        : m_Count(count), m_IndexType(IndexType::UInt32)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        m_Buffer = device->newBuffer(indices, count * sizeof(uint32_t), MTL::ResourceOptionCPUCacheModeDefault);
        if (!m_Buffer)
        {
            CORE_LOG_ERROR("Failed to create Metal index buffer (uint32)!");
        }
    }

    MetalIndexBuffer::~MetalIndexBuffer()
    {
        if (m_Buffer)
            m_Buffer->release();
    }

    void MetalIndexBuffer::Bind() const
    {
        /* Metal中索引缓冲区在绘制时绑定 */
    }

    void MetalIndexBuffer::Unbind() const
    {
        /* Metal中无需解绑 */
    }
}

#endif /* PLATFORM_MACOS */
