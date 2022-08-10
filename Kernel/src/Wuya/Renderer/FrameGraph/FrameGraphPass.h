#pragma once
#include <glm/glm.hpp>
#include "FrameGraphResources.h"

namespace Wuya
{
	class RenderPassNode;

	/* FrameGraphPass信息 */
	struct FrameGraphPassInfo
	{
		static constexpr uint32_t MAX_ATTACHMENT_NUM = MAX_COLOR_ATTACHMENT_NUM + 2;
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
			glm::vec4 ClearColor{};
			/* Sample */
			uint8_t Samples{ 1 };
			/* 视口区域 */
			ViewportRegion ViewportRegion{};
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
		void SetRenderPassNode(const SharedPtr<RenderPassNode>& node) { m_pNode = node; }
		/* 获取RenderPassNode */
		const SharedPtr<RenderPassNode>& GetRenderPassNode() const { return m_pNode; }

		/* 执行阶段 */
		virtual void Execute(const FrameGraphResources& resources) = 0;

	protected:
		SharedPtr<RenderPassNode> m_pNode{ nullptr };
	};


	/* 不带执行阶段的Pass */
	template<typename Data>
	class FrameGraphPass : public IFrameGraphPass
	{
	public:
		/* 数据 */
		void SetData(const Data& data) { m_Data = data; }
		const Data& GetData() const { return m_Data; }
		
		/* 执行阶段 */
		void Execute(const FrameGraphResources& resources) override {}

	protected:
		/* Pass所需数据 */
		Data m_Data;
	};

	/* 带执行阶段的Pass */
	template<typename Data, typename ExecuteFunc>
	class FrameGraphPass_WithExecute : public FrameGraphPass<Data>
	{
	public:
		FrameGraphPass_WithExecute(ExecuteFunc&& execute_func)
			: m_ExecuteFunc(std::move(execute_func))
		{}

		/* 执行阶段 */
		void Execute(const FrameGraphResources& resources) override
		{
			m_ExecuteFunc(resources, this->m_Data);
		}

	protected:
		/* Pass执行阶段函数 */
		ExecuteFunc m_ExecuteFunc;
	};
}
