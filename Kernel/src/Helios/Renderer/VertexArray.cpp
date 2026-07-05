#include "Pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLVertexArray.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalVertexArray.h"
#endif

namespace Helios
{
	SharedPtr<VertexArray> VertexArray::Create()
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLVertexArray>();
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalVertexArray>();
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
