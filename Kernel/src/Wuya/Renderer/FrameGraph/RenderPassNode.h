#pragma once
#include <unordered_set>
#include <glm/glm.hpp>
#include "FrameGraph.h"
#include "FrameGraphResourceHandle.h"
#include "FrameGraphResources.h"

namespace Wuya
{
	class IFrameGraphPass;
	class RenderResourceNode;
	class FrameBuffer;

	/* ����RenderPass��������ݣ�һ��RenderPass���ܰ����������� */
	struct RenderPassData
	{
		std::string DebugName{};																						/* ������ */
		FrameGraphPassInfo::Descriptor Descriptor{};																	/* ���� */
		SharedPtr<RenderResourceNode> IncomingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};				/* �������Դ�ڵ� */
		SharedPtr<RenderResourceNode> OutgoingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};				/* �������Դ�ڵ� */
		FrameGraphResourceHandleTyped<FrameGraphTexture> ValidAttachments[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {}; /* �ռ���Ч��Attachments */
		RenderBufferUsage RenderBufferUsage{ RenderBufferUsage::None };													/* RenderBuffer��ʹ����� */
		SharedPtr<FrameBuffer> FrameBuffer{ nullptr };																	/* FrameBuffer */
	};

	/* RenderPassNode�࣬��ΪFrameGraph������ϵͼ�е�һ���ڵ� */
	class RenderPassNode : public DependencyGraph::Node, public std::enable_shared_from_this<RenderPassNode>
	{
	public:
		RenderPassNode(const std::string& name, FrameGraph& frame_graph, const SharedPtr<IFrameGraphPass>& frame_graph_pass);
		virtual ~RenderPassNode() override;

		/* ��ȡPass�� */
		const std::string& GetDebugName() const { return DebugName; }

		/* �Ե�ǰRenderPass, ��ִ��֮ǰ����Ҫ������Ҫ����Դ����֮���RenderPass������ʹ�õ���Դ��Ӧ��ʱ���١� */
		std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() { return m_ResourcesNeedPrepared; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() const { return m_ResourcesNeedPrepared; }
		std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() { return m_ResourcesNeedDestroy; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() const { return m_ResourcesNeedDestroy; }

		/* ע����ԴHandle */
		void RegisterResourceHandle(FrameGraphResourceHandle handle);
		/* �ж���ԴHandle�Ƿ�ע�� */
		[[nodiscard]] bool IsResourceHandleRegistered(FrameGraphResourceHandle handle) const;

		/* ������������RenderPassData */
		uint32_t CreateRenderPassData(const std::string& name, const FrameGraphPassInfo::Descriptor& desc);
		/* ��ȡRenderPassData */
		[[nodiscard]] SharedPtr<RenderPassData> GetRenderPassData(uint32_t idx) const;

		/* �����׶� */
		void Build();
		/* ִ�н׶� */
		void Execute(const FrameGraphResources& resources);
		/* ���� */
		void Destroy();

	private:
		/* ������ӻ����� */
		[[nodiscard]] std::string Graphvizify() const override;
		[[nodiscard]] std::string GraphvizifyConnectionColor() const override;

		/* ����FrameGraph */
		FrameGraph& m_OwnerFrameGraph;
		/* ������FrameGraphPass */
		SharedPtr<IFrameGraphPass> m_OwnerFrameGraphPass{ nullptr };
		/* ��ע�����ԴHandles */
		std::unordered_set<uint16_t> m_RegisteredHandleIndices{};
		/* ��Ҫ׼������Դ��������ָ�룬�ڴ���FrameGraphͳһ���� */
		std::vector<SharedPtr<IResource>> m_ResourcesNeedPrepared{};
		/* ��Ҫ�ͷŵ���Դ��������ָ�룬�ڴ���FrameGraphͳһ���� */
		std::vector<SharedPtr<IResource>> m_ResourcesNeedDestroy{};
		/*��ǰPass���� */
		std::vector<SharedPtr<RenderPassData>> m_RenderPassDatas{};
	};
}
