#pragma once
#include "Wuya/Renderer/FrameBuffer.h"

namespace Wuya
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferDescription& desc);
		~OpenGLFrameBuffer() override;

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachment_index, int x, int y) override; // ��ȡָ������x,y������λ�õ���ɫֵ

		void ClearColorAttachment(uint32_t attachment_index, int value) override;
		uint32_t GetColorAttachmentByIndex(uint32_t index) const override;
		const FrameBufferDescription& GetDescription() const override { return m_Description; }

	private:
		void Invalidate();
		void ReleaseAttachments();

		uint32_t m_FrameBufferId{ 0 };
		FrameBufferDescription m_Description{};

		// Color
		std::vector<uint32_t> m_ColorAttachments{};
		std::vector<FrameBufferTarget> m_ColorTargets{};

		// Depth
		uint32_t m_DepthAttachment{ 0 };
		FrameBufferTarget m_DepthTarget{ FrameBufferTargetFormat::None };
	};
}
