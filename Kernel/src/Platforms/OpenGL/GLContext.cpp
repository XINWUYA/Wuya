#include "pch.h"
#include "GLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Wuya
{
	GLContext::GLContext(GLFWwindow* window)
		: m_pGLFWWindow(window)
	{
		CORE_ASSERT(window, "GLFWwindow is nullptr!");
	}

	void GLContext::Init()
	{
		glfwMakeContextCurrent(m_pGLFWWindow);

		// ³õÊ¼»¯Glad
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "Failed to init Glad!");

		CORE_LOG_INFO("Using OpenGL:");
		CORE_LOG_INFO("    Vendor: {0}", glGetString(GL_VENDOR));
		CORE_LOG_INFO("    Renderer: {0}", glGetString(GL_RENDERER));
		CORE_LOG_INFO("    Version: {0}", glGetString(GL_VERSION));

		CORE_ASSERT((GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)),
			"OpenGL Version is too old(need >= 4.5).");
	}

	void GLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_pGLFWWindow);
	}
}
