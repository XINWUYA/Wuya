#include "pch.h"
#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>

namespace Wuya
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding_point)
	{
		glCreateBuffers(1, &m_UniformBufferId);
		glNamedBufferData(m_UniformBufferId, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_UniformBufferId);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_UniformBufferId);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_UniformBufferId, offset, size, data);
	}
}