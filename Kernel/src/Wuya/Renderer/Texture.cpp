#include "Pch.h"
#include "Texture.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLTexture.h"

namespace Wuya
{
	Texture::Texture(std::string name, const TextureDesc& texture_desc)
		: m_DebugName(std::move(name)), m_TextureDesc(texture_desc)
	{
	}

	/* 创建纹理 */
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

	/* 创建纹理 */
	SharedPtr<Texture> Texture::Create(const std::string& path, const TextureLoadConfig& load_config)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(path, load_config);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	/* 默认纹理 */
	SharedPtr<Texture>& Texture::White()
	{
		static SharedPtr<Texture> texture;

		if (!texture)
		{
			// Texture
			constexpr TextureDesc desc{ 2,2 };
			texture = Texture::Create("White", desc);
			uint32_t default_texture_data = 0xffffffff; // White
			texture->SetData(&default_texture_data, {});
		}

		return texture;
	}
}
