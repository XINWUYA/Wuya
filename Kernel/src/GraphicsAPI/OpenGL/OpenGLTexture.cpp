#include "Pch.h"
#include "OpenGLTexture.h"
#include "OpenGLCommon.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Wuya
{
	OpenGLTexture::OpenGLTexture(const std::string& name, const TextureDesc& texture_desc)
		: Texture(name, texture_desc), m_InternalFormat(TranslateToOpenGLTextureFormat(texture_desc.Format))
	{
		PROFILE_FUNCTION();

		if (!!(m_TextureDesc.Usage & TextureUsage::Sampleable))
		{
			/* 创建一个普通纹理或作为颜色rt */

			switch (m_TextureDesc.SamplerType)
			{
			case SamplerType::Sampler2D:
				m_TextureTarget = m_TextureDesc.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
				break;
			case SamplerType::Sampler2DArray:
				m_TextureTarget = GL_TEXTURE_2D_ARRAY;
				break;
			case SamplerType::SamplerCubeMap:
				m_TextureTarget = GL_TEXTURE_CUBE_MAP;
				break;
			case SamplerType::Sampler3D:
				m_TextureTarget = GL_TEXTURE_3D;
				break;
			}

			/* 创建纹理 */
			glGenTextures(1, &m_TextureId);
			glBindTexture(m_TextureTarget, m_TextureId);
			glActiveTexture(GL_TEXTURE0);

			/* 填充格式 */
			switch (m_TextureTarget)
			{
			case GL_TEXTURE_2D:
			case GL_TEXTURE_CUBE_MAP:
				glTexStorage2D(m_TextureTarget, (GLsizei)m_TextureDesc.MipLevels, m_InternalFormat, (GLsizei)m_TextureDesc.Width, (GLsizei)m_TextureDesc.Height);
				break;
			case GL_TEXTURE_3D:
			case GL_TEXTURE_2D_ARRAY:
				glTexStorage3D(m_TextureTarget, (GLsizei)m_TextureDesc.MipLevels, m_InternalFormat, (GLsizei)m_TextureDesc.Width, (GLsizei)m_TextureDesc.Height, (GLsizei)m_TextureDesc.Depth);
				break;
			case GL_TEXTURE_2D_MULTISAMPLE:
				glTexImage2DMultisample(m_TextureTarget, m_TextureDesc.Samples, m_InternalFormat, (GLsizei)m_TextureDesc.Width, (GLsizei)m_TextureDesc.Height, GL_TRUE);
				break;
			default:
				break;
			}

			CHECK_GL_ERROR;
		}
		else
		{
			/* 当作为rt，保存Depth/Stencil等信息时，需创建一个RenderBuffer */

			ASSERT(m_TextureDesc.Usage & (TextureUsage::ColorAttachment | TextureUsage::DepthAttachment | TextureUsage::StencilAttachment));
			ASSERT(m_TextureDesc.MipLevels == 1);
			ASSERT(m_TextureDesc.SamplerType == SamplerType::Sampler2D);

			m_TextureTarget = GL_RENDERBUFFER;
			glGenRenderbuffers(1, &m_TextureId);
			glBindRenderbuffer(GL_RENDERBUFFER, m_TextureId);

			if (m_TextureDesc.Samples > 1)
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_TextureDesc.Samples, m_InternalFormat, (GLsizei)m_TextureDesc.Width, (GLsizei)m_TextureDesc.Height);
			else
				glRenderbufferStorage(GL_RENDERBUFFER, m_InternalFormat, (GLsizei)m_TextureDesc.Width, (GLsizei)m_TextureDesc.Height);

			/* 分配内存后，即可解绑该RenderBuffer */
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			CHECK_GL_ERROR;
		}
	}

	OpenGLTexture::OpenGLTexture(const std::string& path)
	{
	}

	OpenGLTexture::~OpenGLTexture()
	{
		PROFILE_FUNCTION();

		if (!!(m_TextureDesc.Usage & TextureUsage::Sampleable)) /* 作为普通Texture */
		{
			glDeleteTextures(1, &m_TextureId);
		}
		else /* 作为RenderBuffer */
		{
			ASSERT(m_TextureTarget == GL_RENDERBUFFER);
			glDeleteRenderbuffers(1, &m_TextureId);
		}
	}

	/* 绑定当前纹理 */
	void OpenGLTexture::Bind(uint32_t slot)
	{
		PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_TextureId);
	}

	/* 解除纹理绑定 */
	void OpenGLTexture::Unbind()
	{
		PROFILE_FUNCTION();

		glBindTexture(m_TextureTarget, 0);
	}

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_InternalFormat(GL_RGBA8), m_DataFormat(GL_RGBA)
	{
		PROFILE_FUNCTION();

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
		glTextureStorage2D(m_TextureId, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		PROFILE_FUNCTION();

		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_IsLoaded = true;
			m_Width = width;
			m_Height = height;

			GLenum internal_format = 0, data_format = 0;
			if (channels == 4)
			{
				internal_format = GL_RGBA8;
				data_format = GL_RGBA;
			}
			else if (channels == 3)
			{
				internal_format = GL_RGB8;
				data_format = GL_RGB;
			}
			m_InternalFormat = internal_format;
			m_DataFormat = data_format;

			ASSERT(internal_format & data_format, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
			glTextureStorage2D(m_TextureId, 1, internal_format, m_Width, m_Height);

			glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, data_format, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		PROFILE_FUNCTION();

		glDeleteTextures(1, &m_TextureId);
	}

	void OpenGLTexture2D::Bind(uint32_t slot)
	{
		PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_TextureId);
	}

	void OpenGLTexture2D::Unbind()
	{
		PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		PROFILE_FUNCTION();

		const uint32_t channel = (m_DataFormat == GL_RGBA) ? 4 : 3;
		ASSERT((size == m_Width * m_Height * channel), "Texture data is not completed!");

		glTextureSubImage2D(m_TextureId, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
}
