#include "pch.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLContext.h"

namespace Wuya
{
	UniquePtr<IRenderContext> IRenderContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateUniquePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
