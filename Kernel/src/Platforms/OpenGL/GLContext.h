#pragma once
#include <Wuya/Renderer/RenderContext.h>

struct GLFWwindow;

namespace Wuya
{
	class GLContext : public IRenderContext
	{
	public:
		GLContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_pGLFWWindow{ nullptr };
	};
}
