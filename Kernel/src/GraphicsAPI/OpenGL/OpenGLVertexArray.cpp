#include "pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Wuya
{
	OpenGLVertexArray::OpenGLVertexArray()
	{
		PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_VertexArrayId);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	void OpenGLVertexArray::Bind() const
	{
		PROFILE_FUNCTION();

		glBindVertexArray(m_VertexArrayId);
	}

	void OpenGLVertexArray::Unbind() const
	{
		PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	static GLenum TranslateBufferDataTypeToOpenGLBaseType(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Int:      return GL_INT;
		case BufferDataType::Int2:     return GL_INT;
		case BufferDataType::Int3:     return GL_INT;
		case BufferDataType::Int4:     return GL_INT;
		case BufferDataType::Bool:     return GL_BOOL;
		case BufferDataType::Float:    return GL_FLOAT;
		case BufferDataType::Float2:   return GL_FLOAT;
		case BufferDataType::Float3:   return GL_FLOAT;
		case BufferDataType::Float4:   return GL_FLOAT;
		case BufferDataType::Mat3:     return GL_FLOAT;
		case BufferDataType::Mat4:     return GL_FLOAT;
		default:
			ASSERT(false, "Unknown BufferDataType!");
			return 0;
		}
	}

	void OpenGLVertexArray::AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer)
	{
		PROFILE_FUNCTION();

		ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

		glBindVertexArray(m_VertexArrayId);
		vertex_buffer->Bind();

		const auto& layout = vertex_buffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case BufferDataType::Int:
			case BufferDataType::Int2:
			case BufferDataType::Int3:
			case BufferDataType::Int4:
			case BufferDataType::Bool:
			{
				glEnableVertexAttribArray(m_VertexBufferIdx);
				glVertexAttribIPointer(m_VertexBufferIdx,
					element.GetComponentCount(),
					TranslateBufferDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset
				);

				m_VertexBufferIdx++;
				break;
			}
			case BufferDataType::Float:
			case BufferDataType::Float2:
			case BufferDataType::Float3:
			case BufferDataType::Float4:
			{
				glEnableVertexAttribArray(m_VertexBufferIdx);
				glVertexAttribPointer(m_VertexBufferIdx,
					element.GetComponentCount(),
					TranslateBufferDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset
				);

				m_VertexBufferIdx++;
				break;
			}
			case BufferDataType::Mat3:
			case BufferDataType::Mat4:
			{
				const auto count = element.GetComponentCount();
				for (uint32_t i = 0; i < count; ++i)
				{
					glEnableVertexAttribArray(m_VertexBufferIdx);
					glVertexAttribPointer(m_VertexBufferIdx,
						count,
						TranslateBufferDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i)
					);
					glVertexAttribDivisor(m_VertexBufferIdx, 1);

					m_VertexBufferIdx++;
				}
				break;
			}
			default: 
				ASSERT(false, "Unknown BufferDataType!");
			}
		}

		m_VertexCount += vertex_buffer->GetCount();
		m_VertexBuffers.push_back(vertex_buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const SharedPtr<IndexBuffer>& index_buffer)
	{
		PROFILE_FUNCTION();

		glBindVertexArray(m_VertexArrayId);
		index_buffer->Bind();

		m_IndexBuffer = index_buffer;
	}

	const uint32_t OpenGLVertexArray::GetVertexCount() const
	{
		return m_IndexBuffer ? m_IndexBuffer->GetCount() : m_VertexCount;
	}
}
