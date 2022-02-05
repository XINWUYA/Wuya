#include "pch.h"
#include "RenderAPI.h"
#include "GraphicsAPI/OpenGL/OpenGLRenderAPI.h"

namespace Wuya
{
	int RenderAPI::m_API = RenderAPI::OpenGL;

	UniquePtr<RenderAPI> RenderAPI::Create()
	{
		switch (m_API)
		{
		case None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case OpenGL:
			return CreateUniquePtr<OpenGLRenderAPI>();
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
