#pragma once
#include "Wuya/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Wuya
{
	class OpenGLContext : public IRenderContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_pGLFWWindow{ nullptr };
	};
}
