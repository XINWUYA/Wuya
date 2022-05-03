#include "Pch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLVertexArray.h"

namespace Wuya
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
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
