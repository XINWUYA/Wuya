#include "Pch.h"
#include "OpenGLCommon.h"

namespace Wuya
{
	/* 检查GLError信息 */
	void CheckGLError(const char* file, const char* func_name, uint32_t line) noexcept
	{
		PROFILE_FUNCTION();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CORE_LOG_ERROR("OpenGLError: {} in {}:{}, at: {}.", STRINGIFY(error), func_name, line, file);
		}
	}

	/* 检查FrameBuffer状态 */
	void CheckGLFrameBufferStatus(GLenum target, const char* func_name, uint32_t line) noexcept
	{
		PROFILE_FUNCTION();

		GLenum status = glCheckFramebufferStatus(target);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			const char* status_str = "UNKNOWN";
			switch (status)
			{
			case GL_FRAMEBUFFER_UNDEFINED: status_str = "GL_FRAMEBUFFER_UNDEFINED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: status_str = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: status_str = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: status_str = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: status_str = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: status_str = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: status_str = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: status_str = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
			}
			CORE_LOG_ERROR("OpenGLFrameBufferError: {} (0x{:X}) in {}():{}.", status_str, status, func_name, line);
		}
	}}
