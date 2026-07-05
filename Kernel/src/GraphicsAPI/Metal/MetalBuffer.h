#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Renderer/Buffer.h"
#include <Metal/Metal.hpp>

namespace Helios
{
    class MetalVertexBuffer : public VertexBuffer
    {
    public:
        MetalVertexBuffer(uint32_t size);
        MetalVertexBuffer(const void* vertices, uint32_t size);
        ~MetalVertexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        void SetData(const void* data, uint32_t size) override;
        uint32_t GetDataSize() const override { return m_Size; }

        void SetLayout(const VertexBufferLayout& layout) override;
        const VertexBufferLayout& GetLayout() const override { return m_Layout; }

        uint32_t GetVertexCount() const override;

        MTL::Buffer* GetMetalBuffer() const { return m_Buffer; }

    private:
        MTL::Buffer* m_Buffer{ nullptr };
        uint32_t m_Size{ 0 };
        VertexBufferLayout m_Layout;
    };

    class MetalIndexBuffer : public IndexBuffer
    {
    public:
        /* 创建带初始数据的索引缓冲区 */
        MetalIndexBuffer(const void* indices, uint32_t count, IndexType type);
        /* 创建空容量的索引缓冲区（预分配） */
        MetalIndexBuffer(uint32_t count, IndexType type);
        ~MetalIndexBuffer() override;

        void Bind() const override;
        void Unbind() const override;

        uint32_t GetCount() const override { return m_Count; }
        IndexType GetIndexType() const override { return m_IndexType; }

        /* 更新索引缓冲区数据 */
        void SetData(const void* data, uint32_t size) override;

        MTL::Buffer* GetMetalBuffer() const { return m_Buffer; }

    private:
        MTL::Buffer* m_Buffer{ nullptr };
        uint32_t m_Count{ 0 };
        IndexType m_IndexType{ IndexType::UInt32 };
    };
}

#endif /* PLATFORM_MACOS */
