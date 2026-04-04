#include "Pch.h"
#include "RenderAPI.h"
#include "GraphicsAPI/OpenGL/OpenGLRenderAPI.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalRenderAPI.h"
#endif

namespace Wuya
{
#ifdef PLATFORM_MACOS
	/* macOS平台默认使用Metal */
	int RenderAPI::m_API = RenderAPI::Metal;
#else
	/* 其他平台使用OpenGL */
	int RenderAPI::m_API = RenderAPI::OpenGL;
#endif

	SharedPtr<RenderAPI> RenderAPI::Create()
	{
		switch (m_API)
		{
		case None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case OpenGL:
			CORE_LOG_INFO("Creating OpenGL RenderAPI...");
			return CreateSharedPtr<OpenGLRenderAPI>();
#ifdef PLATFORM_MACOS
		case Metal:
			CORE_LOG_INFO("Creating Metal RenderAPI...");
			return CreateSharedPtr<MetalRenderAPI>();
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
