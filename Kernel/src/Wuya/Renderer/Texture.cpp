#include "Pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLTexture.h"

namespace Wuya
{
	SharedPtr<Texture> Texture::Create(const std::string& name, uint32_t width, uint32_t height, uint32_t depth,
		uint8_t mip_levels, uint8_t samples, TextureFormat format, SamplerType sampler, TextureUsage usage)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(width, height, depth, mip_levels, samples, format, sampler, usage);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture2D>(width, height);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture2D>(path);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
}
