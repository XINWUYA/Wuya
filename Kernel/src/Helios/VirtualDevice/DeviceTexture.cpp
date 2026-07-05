#include "Pch.h"
#include "DeviceTexture.h"
#include "Helios/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLTexture.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalTexture.h"
#endif

namespace Helios
{
	DeviceTexture::DeviceTexture(std::string name, const TextureDesc& texture_desc)
		: m_DebugName(std::move(name)), m_TextureDesc(texture_desc)
	{
	}

	/* 创建纹理 */
	SharedPtr<DeviceTexture> DeviceTexture::Create(const std::string& name, const TextureDesc& texture_desc)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(name, texture_desc);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalTexture>(name, texture_desc);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	/* 创建纹理 */
	SharedPtr<DeviceTexture> DeviceTexture::Create(const std::string& path, const TextureLoadConfig& load_config)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLTexture>(path, load_config);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalTexture>(path, load_config);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}
	/* 默认纹理 */
	SharedPtr<DeviceTexture> DeviceTexture::White()
	{
		SharedPtr<DeviceTexture> texture;

		if (!texture)
		{
			// DeviceTexture
			constexpr TextureDesc desc{ 2,2 };
			texture = DeviceTexture::Create("DefaultWhiteTex", desc);
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

	SharedPtr<DeviceTexture> DeviceTexture::Black()
	{
		SharedPtr<DeviceTexture> texture;

		if (!texture)
		{
			// DeviceTexture
			constexpr TextureDesc desc{ 2,2 };
			texture = DeviceTexture::Create("DefaultBlackTex", desc);
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

	SharedPtr<DeviceTexture> DeviceTexture::Normal()
	{
		SharedPtr<DeviceTexture> texture;

		if (!texture)
		{
			// DeviceTexture
			constexpr TextureDesc desc{ 2,2 };
			texture = DeviceTexture::Create("DefaultNormalTex", desc);
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
