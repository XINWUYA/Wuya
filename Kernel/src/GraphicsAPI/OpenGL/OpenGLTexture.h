#pragma once
#include <string>
#include <glad/glad.h>
#include "Wuya/Renderer/Texture.h"
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	/**
	 * \brief ������������������������ͨ����Ҳ������RenderBuffer
	 */
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& name, const TextureDesc& texture_desc);
		OpenGLTexture(const std::string& path);
		~OpenGLTexture() override;

		/* �󶨵�ǰ���� */
		void Bind(uint32_t slot = 0) override;
		/* �������� */
		void Unbind() override;
		/* ����������� */
		void SetData(void* data, uint32_t size) override {}
		
		/* ��ȡ��������·�� */
		const std::string& GetPath() const override { return m_Path; }

		/* ����ID */
		uint32_t GetTextureID() const override { return m_TextureId; }
		/* �����Ƿ�ɹ����� */
		bool IsLoaded() const override { return m_IsLoaded; }

		/* ����operators */
		bool operator==(const Texture& other) const override
		{
			return ((OpenGLTexture&)other).GetTextureID() == m_TextureId;
		}

	private:
		/* Ӳ����TextureId */
		uint32_t m_TextureId{ 0 };
		/* Ӳ����TextureFormat */
		GLenum m_InternalFormat{};
		/* Ӳ����Target */
		GLenum m_TextureTarget{};
		/* ��������·�����������ļ�����ʱ���� */
		std::string m_Path{};
		/* �����Ƿ������ɣ��������ļ�����ʱ���� */
		bool m_IsLoaded{ false };

		friend class OpenGLFrameBuffer;
	};

	/* Texture2D */
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D() override;

		void Bind(uint32_t slot = 0) override;
		void Unbind() override;
		void SetData(void* data, uint32_t size) override;
		
		/* ��ȡ��������·�� */
		const std::string& GetPath() const override { return m_Path; }

		/* ����ID */
		uint32_t GetTextureID() const override { return m_TextureId; }
		/* ����ɹ����� */
		bool IsLoaded() const override { return m_IsLoaded; }

		bool operator==(const Texture& other) const override
		{
			return ((OpenGLTexture2D&)other).GetTextureID() == m_TextureId;
		}

	private:
		std::string m_Path{};
		bool m_IsLoaded{ false };
		uint32_t m_Width{ 0 }, m_Height{ 0 };
		uint32_t m_Depth{ 0 };
		uint8_t m_MipLevels{ 1 };
		uint32_t m_TextureId{ 0 };
		GLenum m_InternalFormat{}, m_DataFormat{};
	};
}
