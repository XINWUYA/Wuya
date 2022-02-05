#pragma once
#include "Buffer.h"


namespace Wuya
{
	class VertexArray
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer) = 0;
		virtual void SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer) = 0;

		virtual const std::vector<SharedPtr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const SharedPtr<IndexBuffer>& GetIndexBuffer() const = 0;

		static SharedPtr<VertexArray> Create();
	};
}
