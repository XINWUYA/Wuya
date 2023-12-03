#pragma once
#include <glm/glm.hpp>
#include "FrameGraphResources.h"

namespace Wuya
{
	class RenderPassNode;

	/* FrameGraphPass信息 */
	struct FrameGraphPassInfo
	{
		static constexpr uint32_t MAX_ATTACHMENT_NUM = MAX_COLOR_ATTACHMENT_NUM + 2; /* 后面两个分别对应Depth和Stencil */
		struct TextureAttachments
		{
			union
			{
				FrameGraphResourceHandleTyped<FrameGraphTexture> AttachmentArray[MAX_ATTACHMENT_NUM] = {};

				struct
				{
					FrameGraphResourceHandleTyped<FrameGraphTexture> ColorAttachments[MAX_COLOR_ATTACHMENT_NUM];
					FrameGraphResourceHandleTyped<FrameGraphTexture> DepthAttachment;
					FrameGraphResourceHandleTyped<FrameGraphTexture> StencilAttachment;
				};
			};
		};

		struct Descriptor
		{
			/* Attachments */
			TextureAttachments Attachments{};
			/* ClearColor */
			glm::vec4 ClearColor{ 0.0f,0.0f,0.0f,0.0f };
			/* Sample */
			uint8_t Samples{ 1 };
			/* 视口区域 */
			ViewportRegion ViewportRegion{ 0,0,1,1 };
		};

		/* Pass索引 */
		uint32_t Idx{ 0 };
	};

	/* FrameGraphPass基类
	 * 一个FrameGraphPass对应一个RenderPass，包含该RenderPass的私有数据和执行方法
	 */
	class IFrameGraphPass
	{
	public:
		virtual ~IFrameGraphPass() = default;

		/* 设置RenderPassNode */
		void SetRenderPassNode(const SharedPtr<RenderPassNode>& node) { m_pRenderPassNode = node; }
		/* 获取RenderPassNode */
		[[nodiscard]] SharedPtr<RenderPassNode> GetRenderPassNode() const { return m_pRenderPassNode.lock(); }

		/* 执行阶段 */
		virtual void Execute(const FrameGraphResources& resources) = 0;
		virtual void BeforeExecute();
		virtual void AfterExecute();

	protected:
		/* 对应的RenderPassNode */
		WeakPtr<RenderPassNode> m_pRenderPassNode;
	};

	/* 带执行阶段的FrameGraphPass */
	template<typename Data, typename ExecuteFunc>
	class FrameGraphPass : public IFrameGraphPass
	{
	public:
		FrameGraphPass(ExecuteFunc&& execute_func)
			: m_ExecuteFunc(std::move(execute_func))
		{}
		~FrameGraphPass() override = default;

		/* 数据 */
		void SetData(const Data& data) { m_Data = data; }
		const Data& GetData() const { return m_Data; }


		/* 执行阶段 */
		void Execute(const FrameGraphResources& resources) override
		{
			BeforeExecute();
			m_ExecuteFunc(resources, m_Data);
			AfterExecute();
		}

	protected:
		/* Pass所需数据 */
		Data m_Data;
		/* Pass执行阶段函数 */
		ExecuteFunc m_ExecuteFunc;
	};
}
