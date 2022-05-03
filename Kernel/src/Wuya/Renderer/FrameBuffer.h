#pragma once

namespace Wuya
{
	enum class FrameBufferTargetFormat
	{
		None = 0,

		// Color
		RGBA8,
		RedInteger,

		//Depth/Stencil
		Depth24Stencil8,
	};

	struct FrameBufferTarget
	{
		FrameBufferTargetFormat TextureFormat{ FrameBufferTargetFormat::None };

		FrameBufferTarget(FrameBufferTargetFormat format)
			: TextureFormat(format)
		{}
	};

	struct FrameBufferAttachments
	{
		std::vector<FrameBufferTarget> Targets{};

		FrameBufferAttachments() = default;
		FrameBufferAttachments(std::initializer_list<FrameBufferTarget> targets)
			: Targets(targets)
		{}
	};

	struct FrameBufferDescription
	{
		uint32_t Width{ 0 };
		uint32_t Height{ 0 };
		uint32_t Samples{ 1 };
		bool IsSwapChainTarget{ false };
		FrameBufferAttachments Attachments{};
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachment_index, int x, int y) = 0;

		virtual void ClearColorAttachment(uint32_t attachment_index, int value) = 0;
		virtual uint32_t GetColorAttachmentByIndex(uint32_t index = 0) const = 0;
		virtual const FrameBufferDescription& GetDescription() const = 0;

		static SharedPtr<FrameBuffer> Create(const FrameBufferDescription& desc);

	protected:
		FrameBuffer() = default;
	};
}
