#pragma once
#include "RenderCommon.h"

namespace Wuya
{
	/* 纹理信息描述 */
	struct TextureDesc
	{
		uint32_t Width{ 1 };								/* 宽度 */
		uint32_t Height{ 1 };								/* 高度 */
		uint32_t Depth{ 1 };								/* 深度 */
		uint8_t MipLevels{ 1 };								/* Mip层级 */
		uint8_t Samples{ 0 };								/* 采样次数 */
		TextureFormat Format{ TextureFormat::RGBA8 };		/* 像素格式：RxGxBxAx等 */
		SamplerType SamplerType{ SamplerType::Sampler2D };	/* 采样方式： Sampler2D/2DArray/CubeMap/3D等 */
		TextureUsage Usage{ TextureUsage::Sampleable };		/* 使用方式：ColorAttachment/DepthAttachment/StencilAttachment/Sampleable等 */
	};

	/* 像素数据描述 */
	struct PixelDesc
	{
		PixelFormat Format{ PixelFormat::RGBA };
		PixelType Type{ PixelType::UnsignedByte };
	};

	/* 纹理加载配置 */
	struct TextureLoadConfig
	{
		bool IsFlipV = true;
		bool IsGenMips = true;
		bool IsHdr = false;
		bool IsSrgb = false;
		SamplerType SamplerType{ SamplerType::Sampler2D }; /* 用于决定纹理加载的类型 */
		SamplerWrapMode SamplerWrapMode{ SamplerWrapMode::Repeat };
		SamplerMinFilter SamplerMinFilter{ SamplerMinFilter::Linear };
		SamplerMagFilter SamplerMagFilter{ SamplerMagFilter::Linear };

		bool operator==(const TextureLoadConfig& load_config) const
		{
			return IsFlipV == load_config.IsFlipV
				&& IsGenMips == load_config.IsGenMips
				&& IsHdr == load_config.IsHdr
				&& IsSrgb == load_config.IsSrgb
				&& SamplerType == load_config.SamplerType
				&& SamplerWrapMode == load_config.SamplerWrapMode
				&& SamplerMinFilter == load_config.SamplerMinFilter
				&& SamplerMagFilter == load_config.SamplerMagFilter;
		}

		bool operator!=(const TextureLoadConfig& load_config) const
		{
			return !(*this == load_config);
		}
	};


	/* 通用纹理类，根据描述和用途创建不同类型的纹理 */
	class Texture
	{
	public:
		Texture(std::string name, const TextureDesc& texture_desc);
		virtual ~Texture() = default;

		/* 绑定 */
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;

		/* 填充数据 */
		virtual void SetData(void* data, const PixelDesc& pixel_desc, uint32_t level = 0, 
			uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t offset_z = 0) = 0;

		/* 获取名称 */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }
		/* 获取纹理原始尺寸 */
		[[nodiscard]] uint32_t GetWidth() const	{ return m_TextureDesc.Width; }
		[[nodiscard]] uint32_t GetHeight() const	{ return m_TextureDesc.Height; }
		/* 获取纹理加载配置 */
		[[nodiscard]] const TextureLoadConfig& GetTextureLoadConfig() const { return m_TextureLoadConfig; }

		/* 获取纹理所在路径 */
		[[nodiscard]] virtual const std::string& GetPath() const = 0;
		/* 纹理ID */
		[[nodiscard]] virtual uint32_t GetTextureID() const = 0;
		/* 纹理成功加载 */
		[[nodiscard]] virtual bool IsLoaded() const = 0;

		/* 重载operators */
		virtual bool operator==(const Texture & other) const = 0;

		/* 创建纹理 */
		static SharedPtr<Texture> Create(const std::string& name, const TextureDesc& texture_desc);
		static SharedPtr<Texture> Create(const std::string& path, const TextureLoadConfig& load_config = {}); /* 目前仅支持加载二维纹理 */

		/* 默认纹理 */
		static SharedPtr<Texture> White();
		static SharedPtr<Texture> Black();
		static SharedPtr<Texture> Normal();

	protected:
		Texture() = default;

		/* 名称 */
		std::string m_DebugName{ "Unnamed Texture" };
		/* 纹理描述 */
		TextureDesc m_TextureDesc{};
		/* 加载配置 */
		TextureLoadConfig m_TextureLoadConfig{};
	};
}


