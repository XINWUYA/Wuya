#pragma once
#include "RenderCommon.h"

namespace Wuya
{
	class ITexture
	{
	public:
		ITexture() = default;
		virtual ~ITexture() = default;

		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;

		/* ��ȡ����ԭʼ�ߴ� */
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		/* ��ȡ��������·�� */
		virtual const std::string& GetPath() const = 0;

		/* ����ID */
		virtual uint32_t GetTextureID() const = 0;
		/* ����ɹ����� */
		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const ITexture& other) const = 0;

	};

	/* ͨ�������࣬������������;������ͬ���͵����� */
	class Texture : public ITexture
	{
	public:
		/* �������� */
		static SharedPtr<Texture> Create(const std::string& name, uint32_t width, uint32_t height, uint32_t depth, uint8_t mip_levels, uint8_t samples, TextureFormat format, SamplerType sampler, TextureUsage usage);
	};

	/* ��ͨ2D������ȡ�� */
	class Texture2D : public ITexture
	{
	public:
		static SharedPtr<Texture2D> Create(uint32_t width, uint32_t height);
		static SharedPtr<Texture2D> Create(const std::string& path);
	};
}


