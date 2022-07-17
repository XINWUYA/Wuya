#pragma once
#include <glm/glm.hpp>
#include "FrameGraphResources.h"

namespace Wuya
{
	class RenderPassNode;

	/* FrameGraphPass��Ϣ */
	struct FrameGraphPassInfo
	{
		static constexpr uint32_t MAX_ATTACHMENT_NUM = 8;
		struct TextureAttachments
		{
			union
			{
				FrameGraphResourceHandleTyped<FrameGraphTexture> AttachmentArray[MAX_ATTACHMENT_NUM + 2] = {};

				struct Typed
				{
					FrameGraphResourceHandleTyped<FrameGraphTexture> ColorAttachments[MAX_ATTACHMENT_NUM];
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
			uint8_t Samples{ 0 };
			/* �ӿ����� */
			ViewportRegion ViewportRegion{};
		};

		/* Pass���� */
		uint32_t Idx{ 0 };
	};

	/* FrameGraph��һ��Pass */
	class IFrameGraphPass
	{
	public:
		virtual ~IFrameGraphPass() = default;

		/* ����RenderPassNode */
		void SetRenderPassNode(RenderPassNode* node) { m_pNode = node; }
		/* ��ȡRenderPassNode */
		const RenderPassNode* GetRenderPassNode() const { return m_pNode; }

		/* ִ�н׶� */
		virtual void Execute(const FrameGraphResources& resources) = 0;

	protected:
		RenderPassNode* m_pNode{ nullptr };
	};


	/* ����ִ�н׶ε�Pass */
	template<typename Data>
	class FrameGraphPass : public IFrameGraphPass
	{
	public:
		/* ���� */
		void SetData(const Data& data) { m_Data = data; }
		const Data& GetData() const { return m_Data; }
		
		/* ִ�н׶� */
		void Execute(const FrameGraphResources& resources) override {}

	protected:
		/* Pass�������� */
		Data m_Data;
	};

	/* ��ִ�н׶ε�Pass */
	template<typename Data, typename ExecuteFunc>
	class FrameGraphPass_WithExecute : public FrameGraphPass<Data>
	{
	public:
		FrameGraphPass_WithExecute(ExecuteFunc&& execute_func)
			: m_ExecuteFunc(std::move(execute_func))
		{}

		/* ִ�н׶� */
		void Execute(const FrameGraphResources& resources) override
		{
			m_ExecuteFunc(resources, this->m_Data);
		}

	protected:
		/* Passִ�н׶κ��� */
		ExecuteFunc m_ExecuteFunc;
	};
}
