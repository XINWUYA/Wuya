#pragma once
#include <glm/glm.hpp>
#include "FrameGraphResources.h"

namespace Wuya
{
	class RenderPassNode;

	/* FrameGraphPass��Ϣ */
	struct FrameGraphPassInfo
	{
		static constexpr uint32_t MAX_ATTACHMENT_NUM = MAX_COLOR_ATTACHMENT_NUM + 2; /* ���������ֱ��ӦDepth��Stencil */
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
			/* �ӿ����� */
			ViewportRegion ViewportRegion{ 0,0,1,1 };
		};

		/* Pass���� */
		uint32_t Idx{ 0 };
	};

	/* FrameGraphPass����
	 * һ��FrameGraphPass��Ӧһ��RenderPass��������RenderPass��˽�����ݺ�ִ�з���
	 */
	class IFrameGraphPass
	{
	public:
		virtual ~IFrameGraphPass() = default;

		/* ����RenderPassNode */
		void SetRenderPassNode(const SharedPtr<RenderPassNode>& node) { m_pRenderPassNode = node; }
		/* ��ȡRenderPassNode */
		const SharedPtr<RenderPassNode>& GetRenderPassNode() const { return m_pRenderPassNode; }

		/* ִ�н׶� */
		virtual void Execute(const FrameGraphResources& resources) = 0;

	protected:
		/* ��Ӧ��RenderPassNode */
		SharedPtr<RenderPassNode> m_pRenderPassNode{ nullptr };
	};

	/* ��ִ�н׶ε�FrameGraphPass */
	template<typename Data, typename ExecuteFunc>
	class FrameGraphPass : public IFrameGraphPass
	{
	public:
		FrameGraphPass(ExecuteFunc&& execute_func)
			: m_ExecuteFunc(std::move(execute_func))
		{}

		/* ���� */
		void SetData(const Data& data) { m_Data = data; }
		const Data& GetData() const { return m_Data; }


		/* ִ�н׶� */
		void Execute(const FrameGraphResources& resources) override
		{
			m_ExecuteFunc(resources, m_Data);
		}

	protected:
		/* Pass�������� */
		Data m_Data;
		/* Passִ�н׶κ��� */
		ExecuteFunc m_ExecuteFunc;
	};
}
