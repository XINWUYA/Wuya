#pragma once
#include "RenderCommon.h"

namespace Wuya
{
	class Texture;
	struct PixelDesc;

	/* RenderBuffer��Ϣ */
	struct RenderBufferInfo
	{
		/* ��CreateTexture������RenderBuffer */
		SharedPtr<Texture> RenderTarget{ nullptr };
		/* Mip level */
		uint16_t Level{ 0 };
		/* CubeMap's face or 3DTexture's slice */
		uint16_t Layer{ 0 };
	};

	/* FrameBuffer��Ϣ���� */
	struct FrameBufferDesc
	{
		uint32_t Samples{ 1 };								/* �������� */
		ViewportRegion ViewportRegion;						/* ����Χ */
		RenderBufferUsage Usage;							/* RenderBufferʹ����� */
		std::vector<RenderBufferInfo> ColorRenderBuffers;	/* Color RenderBuffers */
		RenderBufferInfo DepthRenderBuffer;					/* Depth RenderBuffer */
		RenderBufferInfo StencilRenderBuffer;				/* Stencil RenderBuffer */
	};

	/* ������������FrameBuffer */
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		/* �� */
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		/* ��ȡָ������x, y������λ�õ���ɫֵ */
		virtual void ReadPixel(uint32_t attachment_index, int x, int y, const PixelDesc& pixel_desc, void* data) = 0;
		/* ָ����������Attachment */
		virtual void ClearAttachment(uint32_t attachment_index, int level, const PixelDesc & pixel_desc, void* data) = 0;

		/* ��ȡFrameBuffer������Ϣ */
		[[nodiscard]] const FrameBufferDesc& GetDescription() const { return m_FrameBufferDesc; }

		/* ����FrameBuffer */
		static SharedPtr<FrameBuffer> Create(const std::string& name, const FrameBufferDesc& desc);

	protected:
		FrameBuffer() = default;
		FrameBuffer(std::string name, FrameBufferDesc desc);

		/* ���� */
		std::string m_DebugName;
		/* FrameBuffer���� */
		FrameBufferDesc m_FrameBufferDesc;
	};
}
