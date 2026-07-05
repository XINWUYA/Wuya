#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Renderer/VertexArray.h"
#include "MetalBuffer.h"
#include <Metal/Metal.hpp>
#include <vector>

namespace Helios
{
    class MetalVertexArray : public VertexArray
    {
    public:
        MetalVertexArray();
        ~MetalVertexArray() override;

        void Bind() const override;
        void Unbind() const override;

        void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer) override;
        void SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer) override;

        const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        const SharedPtr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

        /* Metal特有接口 */
        void Bind(MTL::RenderCommandEncoder* encoder);
        MTL::VertexDescriptor* GetVertexDescriptor() const { return m_VertexDescriptor; }
        uint32_t GetVertexCount() const override;

    private:
        void BuildVertexDescriptor();

        std::vector<SharedPtr<VertexBuffer>> m_VertexBuffers;
        SharedPtr<IndexBuffer> m_IndexBuffer{ nullptr };
        MTL::VertexDescriptor* m_VertexDescriptor{ nullptr };
    };
}

#endif /* PLATFORM_MACOS */
