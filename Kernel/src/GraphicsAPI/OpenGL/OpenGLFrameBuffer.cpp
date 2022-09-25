#include "Pch.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLCommon.h"
#include <glad/glad.h>

#include "OpenGLTexture.h"

namespace Wuya
{
	constexpr uint32_t MAX_FRAME_TARGET_SIZE = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const std::string& name, const FrameBufferDesc& desc)
		: FrameBuffer(name, desc)
	{
		PROFILE_FUNCTION();

		/* 1. 创建FrameBuffer */
		glGenFramebuffers(1, &m_FrameBufferId);

		/* 2. 附加ColorAttachments */
		if (!!(desc.Usage & RenderBufferUsage::ColorAll))
		{
			GLenum rbos[MAX_COLOR_ATTACHMENT_NUM] = { GL_NONE };
			for (uint32_t i = 0; i < MAX_COLOR_ATTACHMENT_NUM; ++i)
			{
				if (!!(desc.Usage & GetRenderBufferUsageByIndex(i)))
				{
					AttachARenderBuffer(desc.ColorRenderBuffers[i], GL_COLOR_ATTACHMENT0 + i);
					rbos[i] = GL_COLOR_ATTACHMENT0 + i;
				}
			}
			glDrawBuffers(MAX_COLOR_ATTACHMENT_NUM, rbos);

			CHECK_GL_ERROR;
		}

		/* 3. 附加Depth/Stencil Attachment */
		bool is_depth_stencil = false; /* Depth和Stencil附加到同一个RenderBuffer */
		if ((desc.Usage & RenderBufferUsage::DepthStencil) == RenderBufferUsage::DepthStencil)
		{
			AttachARenderBuffer(desc.DepthRenderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
			is_depth_stencil = true;
		}
		
		if (!is_depth_stencil)
		{
			if (!!(desc.Usage & RenderBufferUsage::Depth)) /* 仅附加Depth */
			{
				AttachARenderBuffer(desc.DepthRenderBuffer, GL_DEPTH_ATTACHMENT);
			}

			if (!!(desc.Usage & RenderBufferUsage::Stencil)) /* 仅附加Stencil */
			{
				AttachARenderBuffer(desc.StencilRenderBuffer, GL_STENCIL_ATTACHMENT);
			}
		}

		/* 先解绑 */
		Unbind();

		CHECK_GL_ERROR;
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_FrameBufferId);
	}

	void OpenGLFrameBuffer::Bind()
	{
		PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
		glViewport(m_FrameBufferDesc.ViewportRegion.MinX, m_FrameBufferDesc.ViewportRegion.MinY, (GLsizei)m_FrameBufferDesc.ViewportRegion.Width, (GLsizei)m_FrameBufferDesc.ViewportRegion.Height);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		if (width == 0 || height == 0 || width > MAX_FRAME_TARGET_SIZE || height > MAX_FRAME_TARGET_SIZE)
		{
			CORE_LOG_WARN("Attempted to rezize framebuffer to { 0 }, { 1 }, but not supported!", width, height);
			return;
		}

		m_FrameBufferDesc.ViewportRegion.Width = width;
		m_FrameBufferDesc.ViewportRegion.Height = height;
	}

	/* 读取指定纹理（x, y）像素位置的颜色值 */
	void OpenGLFrameBuffer::ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data)
	{
		PROFILE_FUNCTION();

		ASSERT(attachment_index < m_FrameBufferDesc.ColorRenderBuffers.size());

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);

		glReadPixels(x, y, 1, 1, TranslateToOpenGLPixelFormat(pixel_desc.Format), TranslateToOpenGLPixelType(pixel_desc.Type), data);

		CHECK_GL_ERROR;
	}

	/* 指定数据清理Attachment */
	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachment_index, int level, const PixelDesc& pixel_desc, void* data)
	{
		PROFILE_FUNCTION();

		ASSERT(attachment_index < m_FrameBufferDesc.ColorRenderBuffers.size());

		const auto& render_buffer_info = m_FrameBufferDesc.ColorRenderBuffers[attachment_index];
		const auto& texture = std::dynamic_pointer_cast<OpenGLTexture>(render_buffer_info.RenderTarget);
		
		glClearTexImage(texture->GetTextureID(), level, TranslateToOpenGLPixelFormat(pixel_desc.Format), TranslateToOpenGLPixelType(pixel_desc.Type), data);

		CHECK_GL_ERROR;
	}

	/* 附加一个RenderBuffer到FrameBuffer */
	void OpenGLFrameBuffer::AttachARenderBuffer(const RenderBufferInfo& render_buffer_info, GLenum attachment)
	{
		PROFILE_FUNCTION();

		/* 获取该RenderBuffer的Usage */
		RenderBufferUsage render_buffer_usage{};
		switch (attachment)
		{
		case GL_COLOR_ATTACHMENT0:
		case GL_COLOR_ATTACHMENT1:
		case GL_COLOR_ATTACHMENT2:
		case GL_COLOR_ATTACHMENT3:
		case GL_COLOR_ATTACHMENT4:
		case GL_COLOR_ATTACHMENT5:
		case GL_COLOR_ATTACHMENT6:
		case GL_COLOR_ATTACHMENT7:
			render_buffer_usage = GetRenderBufferUsageByIndex(attachment - GL_COLOR_ATTACHMENT0);
			break;
		case GL_DEPTH_ATTACHMENT:
			render_buffer_usage = RenderBufferUsage::Depth;
			break;
		case GL_STENCIL_ATTACHMENT:
			render_buffer_usage = RenderBufferUsage::Stencil;
			break;
		case GL_DEPTH_STENCIL_ATTACHMENT:
			render_buffer_usage = RenderBufferUsage::DepthStencil;
			break;
		default: 
			break;
		}

		/* 1. 获取rt的格式 */
		const auto& texture = std::dynamic_pointer_cast<OpenGLTexture>(render_buffer_info.RenderTarget);
		GLenum texture_target = GL_TEXTURE_2D;
		if (!!(texture->m_TextureDesc.Usage & TextureUsage::Sampleable)) /* 作为颜色rt */
		{
			switch (texture->m_TextureTarget)
			{
			case GL_TEXTURE_2D:
			case GL_TEXTURE_2D_MULTISAMPLE:
			case GL_TEXTURE_2D_ARRAY:
				texture_target = texture->m_TextureTarget;
				break;
			case GL_TEXTURE_CUBE_MAP:
				texture_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + render_buffer_info.Layer;
				break;
			default:
				break;
			}
		}
		else /* 作为RenderBuffer */
		{
			texture_target = GL_RENDERBUFFER;
		}

		/* 2. 将RenderBuffer附加到FrameBuffer */
		/* 采样次数*/
		if (texture->m_TextureDesc.Samples <= 1)
		{
			PROFILE_SCOPE("Attach RenderBuffer");

			/* 绑定当前FrameBuffer */
			{
				PROFILE_SCOPE("Bind FrameBuffer");

				glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
			}
			/* 附加 */
			if (texture_target == GL_TEXTURE_2D_ARRAY)
			{
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture->m_TextureId, render_buffer_info.Level, render_buffer_info.Layer);
			}
			else /* GL_TEXTURE_2D/GL_TEXTURE_2D_MULTISAMPLE/GL_TEXTURE_CUBE_MAP_POSITIVE_X... */
			{
				PROFILE_SCOPE("FrameBuffer RenderBuffer");

				if(!!(texture->m_TextureDesc.Usage & TextureUsage::Sampleable))
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture_target, texture->m_TextureId, render_buffer_info.Level);
				}
				else
				{
					ASSERT(texture_target == GL_RENDERBUFFER);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, texture_target, texture->m_TextureId);
				}
			}

			CHECK_GL_ERROR;
		}
		else
		{
			/* todo: 支持MultiSample */
			if (!!(texture->m_TextureDesc.Usage & TextureUsage::Sampleable)) /* RenderBuffer不是Sampleable，直接附加 */
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, texture->m_TextureId);
				render_buffer_usage = RenderBufferUsage::None;

				CHECK_GL_ERROR;
			}
			else /* rt需要MultiSample */
			{
				/* 绑定当前FrameBuffer */
				glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);

				/* 需要额外创建一个RenderBuffer, todo: 不需要每次都创建 */
				GLuint rbo = 0;
				glGenRenderbuffers(1, &rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, rbo);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, texture->m_TextureDesc.Samples, texture->m_InternalFormat, (GLsizei)texture->m_TextureDesc.Width, (GLsizei)texture->m_TextureDesc.Height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);

				CHECK_GL_ERROR;
			}
		}
#if 0
		if (!!(texture->m_TextureDesc.Usage & TextureUsage::Sampleable))
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(texture_target, render_buffer_info.RenderTarget->GetTextureID());

			/* 设置使用的Level */
			glTexParameteri(texture_target, GL_TEXTURE_BASE_LEVEL, render_buffer_info.Level);
			glTexParameteri(texture_target, GL_TEXTURE_MAX_LEVEL, render_buffer_info.Level);
		}
#endif
		CHECK_GL_FRAMEBUFFER_STATUS(GL_FRAMEBUFFER);
	}
}
