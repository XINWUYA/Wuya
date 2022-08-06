#pragma once
#include "RenderCommon.h"

namespace Wuya
{
	class Texture;

	/* RenderBuffer信息 */
	struct RenderBufferInfo
	{
		/* 经CreateTexture创建的RenderBuffer */
		SharedPtr<Texture> RenderTarget{ nullptr };
		/* Mip level */
		uint16_t Level{ 0 };
		/* CubeMap's face or 3DTexture's slice */
		uint16_t Layer{ 0 };
	};

	/* FrameBuffer信息描述 */
	struct FrameBufferDesc
	{
		uint32_t Width{ 0 };								/* 宽度 */
		uint32_t Height{ 0 };								/* 高度 */
		uint32_t Samples{ 1 };								/* 采样次数 */
		RenderBufferUsage Usage;							/* RenderBuffer使用情况 */
		std::vector<RenderBufferInfo> ColorRenderBuffers;	/* Color RenderBuffers */
		RenderBufferInfo DepthRenderBuffer;					/* Depth RenderBuffer */
		RenderBufferInfo StencilRenderBuffer;				/* Stencil RenderBuffer */
	};

	/* 根据描述创建FrameBuffer */
	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		/* 绑定 */
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		/* 读取指定纹理（x, y）像素位置的颜色值 */
		virtual int ReadPixel(uint32_t attachment_index, int x, int y) = 0;

		/* 获取FrameBuffer描述信息 */
		const FrameBufferDesc& GetDescription() const { return m_FrameBufferDesc; }

		/* 创建FrameBuffer */
		static SharedPtr<FrameBuffer> Create(const std::string& name, const FrameBufferDesc& desc);

	protected:
		FrameBuffer() = default;
		FrameBuffer(const std::string& name, const FrameBufferDesc& desc);

		/* 名称 */
		std::string m_DebugName{ "Unnamed FrameBuffer" };
		/* FrameBuffer描述 */
		FrameBufferDesc m_FrameBufferDesc{};
	};
}
