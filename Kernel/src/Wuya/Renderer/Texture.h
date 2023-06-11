#pragma once
#include "RenderCommon.h"

namespace Wuya
{
	/* ������Ϣ���� */
	struct TextureDesc
	{
		uint32_t Width{ 1 };								/* ��� */
		uint32_t Height{ 1 };								/* �߶� */
		uint32_t Depth{ 1 };								/* ��� */
		uint8_t MipLevels{ 1 };								/* Mip�㼶 */
		uint8_t Samples{ 0 };								/* �������� */
		TextureFormat Format{ TextureFormat::RGBA8 };		/* ���ظ�ʽ��RxGxBxAx�� */
		SamplerType SamplerType{ SamplerType::Sampler2D };	/* ������ʽ�� Sampler2D/2DArray/CubeMap/3D�� */
		TextureUsage Usage{ TextureUsage::Sampleable };		/* ʹ�÷�ʽ��ColorAttachment/DepthAttachment/StencilAttachment/Sampleable�� */
	};

	/* ������������ */
	struct PixelDesc
	{
		PixelFormat Format{ PixelFormat::RGBA };
		PixelType Type{ PixelType::UnsignedByte };
	};

	/* ����������� */
	struct TextureLoadConfig
	{
		bool IsFlipV = true;
		bool IsGenMips = true;
		bool IsHdr = false;
		bool IsSrgb = false;
		SamplerType SamplerType{ SamplerType::Sampler2D }; /* ���ھ���������ص����� */
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


	/* ͨ�������࣬������������;������ͬ���͵����� */
	class Texture
	{
	public:
		Texture(std::string name, const TextureDesc& texture_desc);
		virtual ~Texture() = default;

		/* �� */
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;

		/* ������� */
		virtual void SetData(void* data, const PixelDesc& pixel_desc, uint32_t level = 0, 
			uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t offset_z = 0) = 0;

		/* ��ȡ���� */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }
		/* ��ȡ����ԭʼ�ߴ� */
		[[nodiscard]] uint32_t GetWidth() const	{ return m_TextureDesc.Width; }
		[[nodiscard]] uint32_t GetHeight() const	{ return m_TextureDesc.Height; }
		/* ��ȡ����������� */
		[[nodiscard]] const TextureLoadConfig& GetTextureLoadConfig() const { return m_TextureLoadConfig; }

		/* ��ȡ��������·�� */
		[[nodiscard]] virtual const std::string& GetPath() const = 0;
		/* ����ID */
		[[nodiscard]] virtual uint32_t GetTextureID() const = 0;
		/* ����ɹ����� */
		[[nodiscard]] virtual bool IsLoaded() const = 0;

		/* ����operators */
		virtual bool operator==(const Texture & other) const = 0;

		/* �������� */
		static SharedPtr<Texture> Create(const std::string& name, const TextureDesc& texture_desc);
		static SharedPtr<Texture> Create(const std::string& path, const TextureLoadConfig& load_config = {}); /* Ŀǰ��֧�ּ��ض�ά���� */

		/* Ĭ������ */
		static SharedPtr<Texture> White();
		static SharedPtr<Texture> Black();
		static SharedPtr<Texture> Normal();

	protected:
		Texture() = default;

		/* ���� */
		std::string m_DebugName{ "Unnamed Texture" };
		/* �������� */
		TextureDesc m_TextureDesc{};
		/* �������� */
		TextureLoadConfig m_TextureLoadConfig{};
	};
}


