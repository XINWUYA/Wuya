#include "Pch.h"
#include "UniformBuffer.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLUniformBuffer.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalUniformBuffer.h"
#endif

namespace Wuya
{
	SharedPtr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding_point)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLUniformBuffer>(size, binding_point);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalUniformBuffer>(size, binding_point);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
