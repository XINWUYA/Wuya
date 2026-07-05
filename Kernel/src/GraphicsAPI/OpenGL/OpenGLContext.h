#pragma once
#include "Helios/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Helios
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
