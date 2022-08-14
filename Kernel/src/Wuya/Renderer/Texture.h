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
		TextureUsage Usage;									/* ʹ�÷�ʽ��ColorAttachment/DepthAttachment/StencilAttachment/Sampleable�� */
	};

	/* ͨ�������࣬������������;������ͬ���͵����� */
	class Texture
	{
	public:
		Texture(const std::string& name, const TextureDesc& texture_desc);
		virtual ~Texture() = default;

		/* �� */
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;

		/* ������� */
		virtual void SetData(void* data, uint32_t size) = 0;

		/* ��ȡ����ԭʼ�ߴ� */
		uint32_t GetWidth() const	{ return m_TextureDesc.Width; }
		uint32_t GetHeight() const	{ return m_TextureDesc.Height; }

		/* ��ȡ��������·�� */
		virtual const std::string& GetPath() const = 0;

		/* ����ID */
		virtual uint32_t GetTextureID() const = 0;
		/* ����ɹ����� */
		virtual bool IsLoaded() const = 0;

		/* ����operators */
		virtual bool operator==(const Texture & other) const = 0;

		/* �������� */
		static SharedPtr<Texture> Create(const std::string& name, const TextureDesc& texture_desc);
		static SharedPtr<Texture> Create(const std::string& path);

	protected:
		Texture() = default;

		/* ���� */
		std::string m_DebugName{ "Unnamed Texture" };
		/* �������� */
		TextureDesc m_TextureDesc{};
	};

	/* ��ͨ2D������ȡ�� */
	class Texture2D : public Texture
	{
	public:
		static SharedPtr<Texture2D> Create(uint32_t width, uint32_t height);
		static SharedPtr<Texture2D> Create(const std::string& path);
	};
}


