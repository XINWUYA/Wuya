#include "Pch.h"
#include "RenderContext.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLContext.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalWindow.h"
#endif

namespace Helios
{
	UniquePtr<IRenderContext> IRenderContext::Create(void* window)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateUniquePtr<OpenGLContext>(static_cast<GLFWwindow*>(window));
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateUniquePtr<MetalContext>(static_cast<GLFWwindow*>(window));
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
