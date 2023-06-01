#pragma once
#include "Wuya/Renderer/Buffer.h"

namespace Wuya
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		~OpenGLVertexBuffer() override;

		/* 绑定 */
		void Bind() const override;
		void Unbind() const override;

		/* 设置顶点数据 */
		void SetData(const void* data, uint32_t size) override;
		[[nodiscard]] uint32_t GetDataSize() const override { return m_DataSize; }

		/* 设置顶点布局 */
		void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }
		[[nodiscard]] const VertexBufferLayout& GetLayout() const override { return m_Layout; }

		/* 获取顶点数量 */
		[[nodiscard]] uint32_t GetVertexCount() const override;

	private:
		/* GPU上对应的BufferId */
		uint32_t m_VertexBufferId{ 0 };
		/* 顶点数据的大小 */
		uint32_t m_DataSize{ 0 };
		/* 顶点布局 */
		VertexBufferLayout m_Layout{};
	};


	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer() override;

		/* 绑定 */
		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_IndexBufferId{ 0 };
		uint32_t m_Count{ 0 };
	};
}

