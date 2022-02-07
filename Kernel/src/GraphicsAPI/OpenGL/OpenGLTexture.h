#pragma once
#include <string>
#include "Wuya/Renderer/Texture.h"
#include <glad/glad.h>
namespace Wuya
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		~OpenGLTexture2D() override;

		void Bind(uint32_t slot = 0) override;
		void SetData(void* data, uint32_t size) override;

		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		uint32_t GetTextureID() const override { return m_TextureId; }
		bool IsLoaded() const override { return m_IsLoaded; }

		bool operator==(const ITexture& other) const override
		{
			return ((OpenGLTexture2D&)other).GetTextureID() == m_TextureId;
		}

	private:
		std::string m_Path{};
		bool m_IsLoaded{ false };
		uint32_t m_Width{ 0 }, m_Height{ 0 };
		uint32_t m_TextureId{ 0 };
		GLenum m_InternalFormat{}, m_DataFormat{};
	};
}
