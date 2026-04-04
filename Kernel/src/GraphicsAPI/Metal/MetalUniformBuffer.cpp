#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalUniformBuffer.h"
#include "MetalCommon.h"
#include "MetalRenderAPI.h"
#include "Wuya/Renderer/Renderer.h"

namespace Wuya
{
    MetalUniformBuffer::MetalUniformBuffer(uint32_t size, uint32_t binding_point)
        : m_Size(size), m_BindingPoint(binding_point)
    {
        PROFILE_FUNCTION();

        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
        {
            CORE_LOG_ERROR("MetalUniformBuffer: Failed to get Metal device");
            return;
        }

        /* 创建缓冲区，使用Shared存储模式以便CPU写入 */
        m_Buffer = device->newBuffer(size, MTL::ResourceStorageModeShared);
        if (!m_Buffer)
        {
            CORE_LOG_ERROR("MetalUniformBuffer: Failed to create Metal buffer!");
        }
    }

    MetalUniformBuffer::~MetalUniformBuffer()
    {
        PROFILE_FUNCTION();

        if (m_Buffer)
            m_Buffer->release();
    }

    void MetalUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        PROFILE_FUNCTION();

        if (!m_Buffer)
        {
            CORE_LOG_ERROR("MetalUniformBuffer: Buffer is null!");
            return;
        }

        if (offset + size > m_Size)
        {
            CORE_LOG_ERROR("MetalUniformBuffer: Data size exceeds buffer size!");
            return;
        }

        /* 获取缓冲区内容指针并拷贝数据 */
        /* 注意：Shared存储模式下，CPU和GPU共享内存，不需要调用didModifyRange() */
        void* buffer_data = m_Buffer->contents();
        if (buffer_data)
        {
            memcpy(static_cast<uint8_t*>(buffer_data) + offset, data, size);
        }
    }
}

#endif /* PLATFORM_MACOS */
