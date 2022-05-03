#include "Pch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLBuffer.h"

namespace Wuya
{
	static uint32_t GetBufferDataTypeSize(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Int:		return 4;
		case BufferDataType::Int2:		return 4 * 2;
		case BufferDataType::Int3:		return 4 * 3;
		case BufferDataType::Int4:		return 4 * 4;
		case BufferDataType::Bool:		return 1;
		case BufferDataType::Float:		return 4;
		case BufferDataType::Float2:	return 4 * 2;
		case BufferDataType::Float3:	return 4 * 3;
		case BufferDataType::Float4:	return 4 * 4;
		case BufferDataType::Mat3:		return 4 * 3 * 3;
		case BufferDataType::Mat4:		return 4 * 4 * 4;
		default:
			ASSERT(false, "Unknown BufferDataType!");
			return 0;
		}
	}

	BufferElement::BufferElement(const std::string& name, BufferDataType type, bool normalized)
		: Name(name), Type(type), Size(GetBufferDataTypeSize(type)), Offset(0), Normalized(normalized)
	{}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch (Type)
		{
		case BufferDataType::Int:		return 1;
		case BufferDataType::Int2:		return 2;
		case BufferDataType::Int3:		return 3;
		case BufferDataType::Int4:		return 4;
		case BufferDataType::Bool:		return 1;
		case BufferDataType::Float:		return 1;
		case BufferDataType::Float2:	return 2;
		case BufferDataType::Float3:	return 3;
		case BufferDataType::Float4:	return 4;
		case BufferDataType::Mat3:		return 3; // Mat need process specially
		case BufferDataType::Mat4:		return 4; // Mat need process specially
		default: 
			ASSERT(false, "Unknown BufferDataType!");
			return 0;
		}
	}

	VertexBufferLayout::VertexBufferLayout(std::initializer_list<BufferElement> elements)
		: m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	void VertexBufferLayout::CalculateOffsetsAndStride()
	{
		m_Stride = 0;
		size_t offset = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	SharedPtr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLVertexBuffer>(size);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<VertexBuffer> VertexBuffer::Create(const float* vertices, uint32_t size)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLVertexBuffer>(vertices, size);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLIndexBuffer>(indices, count);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
