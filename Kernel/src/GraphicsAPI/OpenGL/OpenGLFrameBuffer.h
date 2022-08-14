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

		/* 读取指定纹理（x, y）像素位置的颜色值 */
		int ReadPixel(uint32_t attachment_index, int x, int y) override;

	private:
		/* 附加一个RenderBuffer到FrameBuffer */
		void AttachARenderBuffer(const RenderBufferInfo& render_buffer_info, GLenum attachment);

		uint32_t m_FrameBufferId{ 0 };
	};
}
