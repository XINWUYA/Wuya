#include "Pch.h"
#include "DeviceQueryNode.h"
#include "Helios/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLQueryNode.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalQuery.h"
#endif

namespace Helios
{
	DeviceQueryNode* DeviceQueryNode::Create()
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			break;
		case RenderAPI::OpenGL:
			return new OpenGLQueryNode;
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return new MetalQueryNode;
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			break;
		}
		return nullptr;
	}
}