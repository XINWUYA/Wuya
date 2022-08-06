#include "Pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLTexture.h"

namespace Wuya
{
	Texture::Texture(const std::string& name, const TextureDesc& texture_desc)
		: m_DebugName(name), m_TextureDesc(texture_desc)
	{
	}

	SharedPtr<Texture> Texture::Create(const std::string& name, const TextureDesc& texture_desc)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(name, texture_desc);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<Texture> Texture::Create(const std::string& path)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(path);
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
