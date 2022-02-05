#pragma once
#include "Wuya/Renderer/VertexArray.h"

namespace Wuya
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() override;

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer) override;
		void SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer) override;

		const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const SharedPtr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		uint32_t m_VertexArrayId;
		uint32_t m_VertexBufferIdx{ 0 };
		std::vector<SharedPtr<VertexBuffer>> m_VertexBuffers;
		SharedPtr<IndexBuffer> m_IndexBuffer;
	};
}

