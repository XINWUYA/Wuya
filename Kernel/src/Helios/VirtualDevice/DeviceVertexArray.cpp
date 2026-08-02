#include "Pch.h"
#include "DeviceVertexArray.h"
#include "Helios/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLVertexArray.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalVertexArray.h"
#endif

namespace Helios
{
	SharedPtr<DeviceVertexArray> DeviceVertexArray::Create(const std::string& name)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLVertexArray>(name);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalVertexArray>(name);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
