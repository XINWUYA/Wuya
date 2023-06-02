#include "Pch.h"
#include "OpenGLCommon.h"

namespace Wuya
{
	/* ���GLError ��Ϣ */
	void CheckGLError(const char* file, const char* func_name, uint32_t line) noexcept
	{
		PROFILE_FUNCTION();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CORE_LOG_ERROR("OpenGLError: {} in {}:{}, at: {}.", STRINGIFY(error), func_name, line, file);
		}
	}

	/* ���FrameBuffer״̬ */
	void CheckGLFrameBufferStatus(GLenum target, const char* func_name, uint32_t line) noexcept
	{
		PROFILE_FUNCTION();

		GLenum status = glCheckFramebufferStatus(target);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			CORE_LOG_ERROR("OpenGLFrameBufferError: {} in {}():{}.", STRINGIFY(status), func_name, line);
		}
	}
}
