#pragma once
#include "Wuya/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Wuya
{
	class OpenGLContext final : public IRenderContext
	{
	public:
		OpenGLContext(GLFWwindow* window);
		~OpenGLContext() override = default;

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_pGLFWWindow{ nullptr };
	};
}
