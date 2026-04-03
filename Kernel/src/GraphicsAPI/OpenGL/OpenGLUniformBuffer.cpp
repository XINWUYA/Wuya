#include "Pch.h"
#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>

namespace Wuya
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding_point)
	{
		PROFILE_FUNCTION();

		glGenBuffers(1, &m_UniformBufferId);
		glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBufferId);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_UniformBufferId);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_UniformBufferId);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		PROFILE_FUNCTION();

		glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBufferId);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}
}
