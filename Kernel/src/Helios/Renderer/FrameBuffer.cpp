#include "Pch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLFrameBuffer.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalFrameBuffer.h"
#endif

namespace Helios
{
	FrameBuffer::FrameBuffer(std::string name, FrameBufferDesc desc)
		: m_DebugName(std::move(name)), m_FrameBufferDesc(std::move(desc))
	{
	}

	/* 创建FrameBuffer */
	SharedPtr<FrameBuffer> FrameBuffer::Create(const std::string& name, const FrameBufferDesc& desc)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLFrameBuffer>(name, desc);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalFrameBuffer>(name, desc);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}}
