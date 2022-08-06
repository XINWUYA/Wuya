#include "Pch.h"
#include "OpenGLCommon.h"

namespace Wuya
{
	/* ���GLError ��Ϣ */
	void CheckGLError(const char* func_name, uint32_t line) noexcept
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CORE_LOG_ERROR("OpenGLError: {} in {}:{}.", STRINGIFY(error), func_name, line);
		}
	}

	/* ���FrameBuffer״̬ */
	void CheckGLFrameBufferStatus(GLenum target, const char* func_name, uint32_t line) noexcept
	{
		GLenum status = glCheckFramebufferStatus(target);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			CORE_LOG_ERROR("OpenGLFrameBufferError: {} in {}():{}.", STRINGIFY(status), func_name, line);
		}
	}
}
