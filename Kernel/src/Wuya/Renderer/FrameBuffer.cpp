#include "Pch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLFrameBuffer.h"

namespace Wuya
{
	SharedPtr<FrameBuffer> FrameBuffer::Create(const FrameBufferDescription& desc)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLFrameBuffer>(desc);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
