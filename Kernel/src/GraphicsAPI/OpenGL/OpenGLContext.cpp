#include "Pch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Wuya
{
	OpenGLContext::OpenGLContext(GLFWwindow* window)
		: m_pGLFWWindow(window)
	{
		ASSERT(window, "GLFWwindow is nullptr!");
	}

	void OpenGLContext::Init()
	{
		PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_pGLFWWindow);

		// 初始化Glad
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT(status, "Failed to init Glad!");

		CORE_LOG_INFO("Using OpenGL:");
		CORE_LOG_INFO("    Vendor: {0}", glGetString(GL_VENDOR));
		CORE_LOG_INFO("    Renderer: {0}", glGetString(GL_RENDERER));
		CORE_LOG_INFO("    Version: {0}", glGetString(GL_VERSION));

		ASSERT((GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)),
			"OpenGL Version is too old(need >= 4.5).");


		/* 设置DebugOutput回调 */
#ifdef WUYA_DEBUG
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback([](GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* user_params)
				{
					if (id == 131169 || id == 131185 || id == 131218 || id == 131204)  /* ignore these non-significant error codes */
						return;

					switch (severity)
					{
					case GL_DEBUG_SEVERITY_HIGH:
						CORE_LOG_CRITICAL("OpenGL DebugOutput: {} from Source({}), Type({}), Id({}).", message, STRINGIFY(source), STRINGIFY(type), id);
						break;
					case GL_DEBUG_SEVERITY_MEDIUM:
						CORE_LOG_ERROR("OpenGL DebugOutput: {} from Source({}), Type({}), Id({}).", message, STRINGIFY(source), STRINGIFY(type), id);
						break;
					case GL_DEBUG_SEVERITY_LOW:
						CORE_LOG_WARN("OpenGL DebugOutput: {} from Source({}), Type({}), Id({}).", message, STRINGIFY(source), STRINGIFY(type), id);
						break;
					case GL_DEBUG_SEVERITY_NOTIFICATION:
						CORE_LOG_INFO("OpenGL DebugOutput: {} from Source({}), Type({}), Id({}).", message, STRINGIFY(source), STRINGIFY(type), id);
						break;
					default:
						break;
					}
				}, nullptr);
			//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_TRUE);
			glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_TRUE);
		}
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		PROFILE_FUNCTION();

		glfwSwapBuffers(m_pGLFWWindow);
	}
}
