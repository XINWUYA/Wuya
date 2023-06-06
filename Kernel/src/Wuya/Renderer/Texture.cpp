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
	SharedPtr<Texture> Texture::White()
	{
		SharedPtr<Texture> texture;

		if (!texture)
		{
			// Texture
			constexpr TextureDesc desc{ 2,2 };
			texture = Texture::Create("DefaultWhiteTex", desc);
			uint32_t default_texture_data[4] = {
				0xffffffff,
				0xffffffff,
				0xffffffff,
				0xffffffff,
			}; // White
			texture->SetData(&default_texture_data, {});
		}

		return texture;
	}

	SharedPtr<Texture> Texture::Black()
	{
		SharedPtr<Texture> texture;

		if (!texture)
		{
			// Texture
			constexpr TextureDesc desc{ 2,2 };
			texture = Texture::Create("DefaultBlackTex", desc);
			uint32_t default_texture_data[4] = {
				0x00000000,
				0x00000000,
				0x00000000,
				0x00000000,
			}; // Black
			texture->SetData(default_texture_data, {});
		}

		return texture;
	}

	SharedPtr<Texture> Texture::Normal()
	{
		SharedPtr<Texture> texture;

		if (!texture)
		{
			// Texture
			constexpr TextureDesc desc{ 2,2 };
			texture = Texture::Create("DefaultNormalTex", desc);
			uint32_t default_texture_data[4] = {
				0xffff0000,
				0xffff0000,
				0xffff0000,
				0xffff0000,
			}; // Normal
			texture->SetData(default_texture_data, {});
		}

		return texture;
	}
}
