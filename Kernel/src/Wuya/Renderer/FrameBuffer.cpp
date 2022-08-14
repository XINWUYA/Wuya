#include "Pch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLFrameBuffer.h"

namespace Wuya
{
	FrameBuffer::FrameBuffer(std::string name, FrameBufferDesc desc)
		: m_DebugName(std::move(name)), m_FrameBufferDesc(std::move(desc))
	{
	}

	/* ´´½¨FrameBuffer */
	SharedPtr<FrameBuffer> FrameBuffer::Create(const std::string& name, const FrameBufferDesc& desc)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLFrameBuffer>(name, desc);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
