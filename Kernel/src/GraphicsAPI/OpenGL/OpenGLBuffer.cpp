#include "Pch.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Wuya
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_DataSize(size)
	{
		PROFILE_FUNCTION();

#ifdef __APPLE__
		// macOS: Use traditional functions (OpenGL 4.1 compatible)
		glGenBuffers(1, &m_VertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
#else
		// Windows/Linux: Use modern DSA functions (OpenGL 4.5+)
		glCreateBuffers(1, &m_VertexBufferId);
		glNamedBufferData(m_VertexBufferId, size, nullptr, GL_DYNAMIC_DRAW);
#endif
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
		: m_DataSize(size)
	{
		PROFILE_FUNCTION();

#ifdef __APPLE__
		// macOS: Use traditional functions (OpenGL 4.1 compatible)
		glGenBuffers(1, &m_VertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
#else
		// Windows/Linux: Use modern DSA functions (OpenGL 4.5+)
		glCreateBuffers(1, &m_VertexBufferId);
		glNamedBufferData(m_VertexBufferId, size, vertices, GL_STATIC_DRAW);
#endif
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_VertexBufferId);
	}

	/* 绑定顶点数据 */
	void OpenGLVertexBuffer::Bind() const
	{
		PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
	}

	/* 解绑顶点数据 */
	void OpenGLVertexBuffer::Unbind() const
	{
		PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/* 设置顶点数据 */
	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		PROFILE_FUNCTION();

#ifdef __APPLE__
		// macOS: Use traditional functions
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
#else
		// Windows/Linux: Use modern DSA function
		glNamedBufferSubData(m_VertexBufferId, 0, size, data);
#endif

		m_DataSize = size;
	}

	/* 获取顶点数量 */
	uint32_t OpenGLVertexBuffer::GetVertexCount() const
	{
		PROFILE_FUNCTION();

		ASSERT(!(m_DataSize % m_Layout.GetStride()), "VertexBuffer data is not complete!");
		return m_DataSize / m_Layout.GetStride();
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count)
		: m_Count(count), m_IndexType(IndexType::UInt16)
	{
		PROFILE_FUNCTION();

#ifdef __APPLE__
		// macOS: Use traditional functions (OpenGL 4.1 compatible)
		glGenBuffers(1, &m_IndexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_IndexBufferId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint16_t), indices, GL_STATIC_DRAW);
#else
		// Windows/Linux: Use modern DSA functions (OpenGL 4.5+)
		glCreateBuffers(1, &m_IndexBufferId);
		glNamedBufferData(m_IndexBufferId, count * sizeof(uint16_t), indices, GL_STATIC_DRAW);
#endif
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count), m_IndexType(IndexType::UInt32)
	{
		PROFILE_FUNCTION();

#ifdef __APPLE__
		// macOS: Use traditional functions (OpenGL 4.1 compatible)
		glGenBuffers(1, &m_IndexBufferId);
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
		glBindBuffer(GL_ARRAY_BUFFER, m_IndexBufferId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#else
		// Windows/Linux: Use modern DSA functions (OpenGL 4.5+)
		glCreateBuffers(1, &m_IndexBufferId);
		glNamedBufferData(m_IndexBufferId, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#endif
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_IndexBufferId);
	}

	/* 绑定索引数据 */
	void OpenGLIndexBuffer::Bind() const
	{
		PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferId);
	}

	/* 解绑索引数据 */
	void OpenGLIndexBuffer::Unbind() const
	{
		PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
