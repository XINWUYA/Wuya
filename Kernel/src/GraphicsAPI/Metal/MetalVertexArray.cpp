#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalVertexArray.h"
#include "MetalCommon.h"
#include "MetalRenderAPI.h"
#include "Helios/Renderer/Renderer.h"

namespace Helios
{
    MetalVertexArray::MetalVertexArray(const std::string& name)
    {
        BuildVertexDescriptor();
    }

    MetalVertexArray::~MetalVertexArray()
    {
        if (m_VertexDescriptor)
            m_VertexDescriptor->release();
    }

    void MetalVertexArray::Bind() const
    {
        /* Metal中顶点数组在绘制时绑定 */
    }

    void MetalVertexArray::Unbind() const
    {
        /* Metal中无需解绑 */
    }

    void MetalVertexArray::AddVertexBuffer(const SharedPtr<DeviceVertexBuffer>& vertex_buffer)
    {
        m_VertexBuffers.push_back(vertex_buffer);
        BuildVertexDescriptor();
    }

    void MetalVertexArray::SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer)
    {
        m_IndexBuffer = index_buffer;
    }

    void MetalVertexArray::Bind(MTL::RenderCommandEncoder* encoder)
    {
        if (!encoder)
            return;

        /* 绑定所有顶点缓冲区 */
        for (size_t i = 0; i < m_VertexBuffers.size(); ++i)
        {
            auto metal_buffer = std::dynamic_pointer_cast<MetalVertexBuffer>(m_VertexBuffers[i]);
            if (metal_buffer && metal_buffer->GetMetalBuffer())
            {
                encoder->setVertexBuffer(metal_buffer->GetMetalBuffer(), 0, static_cast<NS::UInteger>(i));
            }
        }
    }

    uint32_t MetalVertexArray::GetVertexCount() const
    {
        if (m_VertexBuffers.empty())
            return 0;

        auto first_buffer = std::dynamic_pointer_cast<MetalVertexBuffer>(m_VertexBuffers[0]);
        return first_buffer ? first_buffer->GetVertexCount() : 0;
    }

    void MetalVertexArray::BuildVertexDescriptor()
    {
        /* 释放旧的描述符 */
        if (m_VertexDescriptor)
        {
            m_VertexDescriptor->release();
            m_VertexDescriptor = nullptr;
        }

        /* 如果没有顶点缓冲区，不创建空的描述符 */
        if (m_VertexBuffers.empty())
        {
            return;
        }

        /* 创建新的顶点描述符 */
        m_VertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        if (!m_VertexDescriptor)
        {
            CORE_LOG_ERROR("Failed to create Metal vertex descriptor!");
            return;
        }

        uint32_t attribute_index = 0;

        /* 遍历所有顶点缓冲区 */
        for (size_t buffer_index = 0; buffer_index < m_VertexBuffers.size(); ++buffer_index)
        {
            auto metal_buffer = std::dynamic_pointer_cast<MetalVertexBuffer>(m_VertexBuffers[buffer_index]);
            if (!metal_buffer)
                continue;

            const auto& layout = metal_buffer->GetLayout();
            const auto& elements = layout.GetElements();

            /* 配置缓冲区布局 */
            auto buffer_layout_descriptor = m_VertexDescriptor->layouts()->object(static_cast<NS::UInteger>(buffer_index));
            buffer_layout_descriptor->setStride(layout.GetStride());
            buffer_layout_descriptor->setStepFunction(MTL::VertexStepFunctionPerVertex);

            /* 配置顶点属性 */
            for (const auto& element : elements)
            {
                auto attribute_descriptor = m_VertexDescriptor->attributes()->object(static_cast<NS::UInteger>(attribute_index));
                attribute_descriptor->setFormat(ToMetalVertexFormat(element.Type));
                attribute_descriptor->setBufferIndex(static_cast<NS::UInteger>(buffer_index));
                attribute_descriptor->setOffset(element.Offset);

                attribute_index++;
            }
        }
    }
}

#endif /* PLATFORM_MACOS */
