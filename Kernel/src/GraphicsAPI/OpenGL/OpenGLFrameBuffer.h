#pragma once
#include <glad/glad.h>
#include "Wuya/Renderer/FrameBuffer.h"

namespace Wuya
{
	/* FrameBuffer类 */
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const std::string& name, const FrameBufferDesc& desc);
		~OpenGLFrameBuffer() override;

		/* 绑定 */
		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		/* 获取指定附件x, y像素位置的颜颜色值 */
		void ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data) override;
		/* 指定某个ColorAttachment */
        void ClearAttachment(uint32_t attachment_index, int level, const PixelDesc& pixel_desc, void* data) override;

	private:
		/* 绑定一个RenderBuffer到FrameBuffer */
		void AttachARenderBuffer(const RenderBufferInfo& render_buffer_info, GLenum attachment);

		uint32_t m_FrameBufferId{ 0 };
	};
}
