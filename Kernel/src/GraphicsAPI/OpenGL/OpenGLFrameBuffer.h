#pragma once
#include <glad/glad.h>
#include "Wuya/Renderer/FrameBuffer.h"

namespace Wuya
{
	/* FrameBuffer�� */
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const std::string& name, const FrameBufferDesc& desc);
		~OpenGLFrameBuffer() override;

		/* �� */
		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		/* ��ȡָ������x, y������λ�õ���ɫֵ */
		void ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data) override;
		/* ָ����������Attachment */
		void ClearAttachment(uint32_t attachment_index, int level, const PixelDesc& pixel_desc, void* data) override;

	private:
		/* ����һ��RenderBuffer��FrameBuffer */
		void AttachARenderBuffer(const RenderBufferInfo& render_buffer_info, GLenum attachment);

		uint32_t m_FrameBufferId{ 0 };
	};
}
