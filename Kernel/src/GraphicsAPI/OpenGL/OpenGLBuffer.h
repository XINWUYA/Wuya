#pragma once
#include <Wuya/Renderer/Buffer.h>

namespace Wuya
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const float* vertices, uint32_t size);
		~OpenGLVertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;

		const VertexBufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }

		uint32_t GetCount() const override;

	private:
		uint32_t m_DataSize{ 0 };
		uint32_t m_VertexBufferId{ 0 };
		VertexBufferLayout m_Layout{};
	};


	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_IndexBufferId{ 0 };
		uint32_t m_Count{ 0 };
	};
}

