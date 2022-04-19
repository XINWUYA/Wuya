#include "pch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Wuya
{
	constexpr uint32_t MAX_FRAME_TARGET_SIZE = 8192;

	static bool IsDepthFormat(FrameBufferTargetFormat format)
	{
		switch (format)
		{
		case FrameBufferTargetFormat::Depth24Stencil8:  
			return true;
		default:
			return false;
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferDescription& desc)
		: m_Description(desc)
	{
		PROFILE_FUNCTION();

		for (auto frame_target : m_Description.Attachments.Targets)
		{
			if (!IsDepthFormat(frame_target.TextureFormat))
				m_ColorTargets.emplace_back(frame_target);
			else
				m_DepthTarget = frame_target;
		}

		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		ReleaseAttachments();
	}

	void OpenGLFrameBuffer::Bind()
	{
		PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
		glViewport(0, 0, m_Description.Width, m_Description.Height);
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

		m_Description.Width = width;
		m_Description.Height = height;

		Invalidate();
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachment_index, int x, int y)
	{
		PROFILE_FUNCTION();

		ASSERT(attachment_index < m_ColorAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);

		int pixel_data;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
		return pixel_data;
	}

	static GLenum TranslateBufferTargetFormatToOpenGLBaseFormat(FrameBufferTargetFormat format)
	{
		switch (format)
		{
		case FrameBufferTargetFormat::RGBA8:
			return GL_RGBA8;
		case FrameBufferTargetFormat::RedInteger: 
			return GL_RED_INTEGER;
		default:
			ASSERT(false);
			return 0;
		}
	}

	void OpenGLFrameBuffer::ClearColorAttachment(uint32_t attachment_index, int value)
	{
		PROFILE_FUNCTION();

		ASSERT(attachment_index < m_ColorAttachments.size());

		auto& target = m_ColorTargets[attachment_index];
		glClearTexImage(m_ColorAttachments[attachment_index],
			0,
			TranslateBufferTargetFormatToOpenGLBaseFormat(target.TextureFormat),
			GL_INT,
			&value
		);
	}

	uint32_t OpenGLFrameBuffer::GetColorAttachmentByIndex(uint32_t index) const
	{
		ASSERT(index < m_ColorAttachments.size());

		return m_ColorAttachments[index];
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		PROFILE_FUNCTION();

		if (m_FrameBufferId)
		{
			ReleaseAttachments();
		}

		glCreateFramebuffers(1, &m_FrameBufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);

		const bool is_multi_sample = m_Description.Samples > 1;
		const GLenum texture_target = is_multi_sample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		// Color attachments
		if (!m_ColorTargets.empty())
		{
			m_ColorAttachments.resize(m_ColorTargets.size());

			glCreateTextures(texture_target, m_ColorAttachments.size(), m_ColorAttachments.data());
			for (auto i = 0; i < m_ColorAttachments.size(); ++i)
			{
				glBindTexture(texture_target, m_ColorAttachments[i]);
				switch (m_ColorTargets[i].TextureFormat)
				{
				case FrameBufferTargetFormat::RGBA8:
				{
					if (is_multi_sample)
					{
						glTexImage2DMultisample(texture_target, m_Description.Samples, GL_RGBA8, m_Description.Width, m_Description.Height, GL_FALSE);
					}
					else
					{
						glTexImage2D(texture_target, 0, GL_RGBA8, m_Description.Width, m_Description.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture_target, m_ColorAttachments[i], 0);
					break;
				}
				case FrameBufferTargetFormat::RedInteger: 
				{
					if (is_multi_sample)
					{
						glTexImage2DMultisample(texture_target, m_Description.Samples, GL_R32I, m_Description.Width, m_Description.Height, GL_FALSE);
					}
					else
					{
						glTexImage2D(texture_target, 0, GL_R32I, m_Description.Width, m_Description.Height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);

						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}

					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture_target, m_ColorAttachments[i], 0);
					break;
				}
				}
			}
		}

		// Depth attachment
		if (m_DepthTarget.TextureFormat != FrameBufferTargetFormat::None)
		{
			glCreateTextures(texture_target, 1, &m_DepthAttachment);
			glBindTexture(texture_target, m_DepthAttachment);

			switch (m_DepthTarget.TextureFormat)
			{
			case FrameBufferTargetFormat::Depth24Stencil8:
			{
				if (is_multi_sample)
				{
					glTexImage2DMultisample(texture_target, m_Description.Samples, GL_DEPTH24_STENCIL8, m_Description.Width, m_Description.Height, GL_FALSE);
				}
				else
				{
					glTexStorage2D(texture_target, 1, GL_DEPTH24_STENCIL8, m_Description.Width, m_Description.Height);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture_target, m_DepthAttachment, 0);
				break;
			}
			}
		}

		// todo: 最大支持4个Color Attachments (可以尝试自适应支持任意数量)
		if (m_ColorAttachments.size() > 1)
		{
			ASSERT(m_ColorAttachments.size() <= 4, "Supports up to 4 color attachments!");
			const GLenum attachments[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
			};
			glDrawBuffers(m_ColorAttachments.size(), attachments);
		}
		else if (m_ColorAttachments.empty())
		{
			// Depth only
			glDrawBuffer(GL_NONE);
		}

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
		Unbind();
	}

	void OpenGLFrameBuffer::ReleaseAttachments()
	{
		PROFILE_FUNCTION();

		glDeleteFramebuffers(1, &m_FrameBufferId);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}
}
