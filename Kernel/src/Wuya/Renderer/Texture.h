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
		TextureUsage Usage;									/* 使用方式：ColorAttachment/DepthAttachment/StencilAttachment/Sampleable等 */
	};

	/* 通用纹理类，根据描述和用途创建不同类型的纹理 */
	class Texture
	{
	public:
		Texture(const std::string& name, const TextureDesc& texture_desc);
		virtual ~Texture() = default;

		/* 绑定 */
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;

		/* 填充数据 */
		virtual void SetData(void* data, uint32_t size) = 0;

		/* 获取纹理原始尺寸 */
		uint32_t GetWidth() const	{ return m_TextureDesc.Width; }
		uint32_t GetHeight() const	{ return m_TextureDesc.Height; }

		/* 获取纹理所在路径 */
		virtual const std::string& GetPath() const = 0;

		/* 纹理ID */
		virtual uint32_t GetTextureID() const = 0;
		/* 纹理成功加载 */
		virtual bool IsLoaded() const = 0;

		/* 重载operators */
		virtual bool operator==(const Texture & other) const = 0;

		/* 创建纹理 */
		static SharedPtr<Texture> Create(const std::string& name, const TextureDesc& texture_desc);
		static SharedPtr<Texture> Create(const std::string& path);

	protected:
		Texture() = default;

		/* 名称 */
		std::string m_DebugName{ "Unnamed Texture" };
		/* 纹理描述 */
		TextureDesc m_TextureDesc{};
	};

	/* 普通2D纹理，待取缔 */
	class Texture2D : public Texture
	{
	public:
		static SharedPtr<Texture2D> Create(uint32_t width, uint32_t height);
		static SharedPtr<Texture2D> Create(const std::string& path);
	};
}


