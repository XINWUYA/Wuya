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
		OpenGLTexture(const std::string& path, const TextureLoadConfig& load_config);
		~OpenGLTexture() override;

		/* �󶨵�ǰ���� */
		void Bind(uint32_t slot = 0) override;
		/* �������� */
		void Unbind() override;
		/* ����������� */
		void SetData(void* data, const PixelDesc& pixel_desc, uint32_t level = 0,
			uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t offset_z = 0) override;
		
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
}
