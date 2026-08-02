#pragma once
#include "Helios/VirtualDevice/DeviceVertexArray.h"

namespace Helios
{
	class OpenGLVertexArray : public DeviceVertexArray
	{
	public:
		OpenGLVertexArray(const std::string& name);
		~OpenGLVertexArray() override;

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const SharedPtr<DeviceVertexBuffer>& vertex_buffer) override;
		void SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer) override;

		const std::vector<SharedPtr<DeviceVertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const SharedPtr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
		uint32_t GetVertexCount() const override;

	private:
		uint32_t m_VertexArrayId{ 0 };
		uint32_t m_VertexBufferIdx{ 0 };
		uint32_t m_VertexCount{ 0 };
		std::vector<SharedPtr<DeviceVertexBuffer>> m_VertexBuffers;
		SharedPtr<IndexBuffer> m_IndexBuffer;
	};
}

