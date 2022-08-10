#pragma once
#include <unordered_set>
#include <glm/glm.hpp>
#include "DependencyGraph.h"
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
		/* ������ */
		std::string Name{};
		/* ���� */
		FrameGraphPassInfo::Descriptor Descriptor{};
		/* �������ݽڵ� */
		SharedPtr<RenderResourceNode> IncomingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* ������ݽڵ� */
		SharedPtr<RenderResourceNode> OutgoingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* �ռ���Ч��Attachments */
		FrameGraphResourceHandleTyped<FrameGraphTexture> ValidAttachments[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* RenderBuffer��ʹ����� */
		RenderBufferUsage RenderBufferUsage{ RenderBufferUsage::None };
		/* FrameBuffer */
		SharedPtr<FrameBuffer> FrameBuffer{ nullptr };
	};

	/* RenderPassNode�࣬��ΪFrameGraph������ϵͼ�е�һ���ڵ� */
	class RenderPassNode : public DependencyGraph::Node, public std::enable_shared_from_this<RenderPassNode>
	{
	public:
		RenderPassNode(FrameGraph& frame_graph, const std::string& name, const SharedPtr<IFrameGraphPass>& frame_graph_pass);
		virtual ~RenderPassNode() override;

		/* ��ȡPass�� */
		const std::string& GetName() const { return Name; }
		/* ��ȡ��Դ */
		std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() { return m_ResourcesNeedPrepared; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() const { return m_ResourcesNeedPrepared; }
		std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() { return m_ResourcesNeedDestroy; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() const { return m_ResourcesNeedDestroy; }

		/* ע����ԴHandle */
		void RegisterResourceHandle(FrameGraphResourceHandle handle);
		/* �ж���ԴHandle�Ƿ�ע�� */
		bool IsResourceHandleRegistered(FrameGraphResourceHandle handle) const;

		/* ����RenderPassData */
		uint32_t CreateRenderPassData(const std::string& name, const FrameGraphPassInfo::Descriptor& desc);
		/* ��ȡRenderPassData */
		const RenderPassData* GetRenderPassData(uint32_t idx) const;
		/* �����׶� */
		void Build();
		/* ִ�н׶� */
		void Execute(const FrameGraphResources& resources);
		/* ���� */
		void Destroy();

	private:
		/* ������ӻ����� */
		std::string Graphvizify() const override;
		std::string GraphvizifyConnectionColor() const override;

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
		std::vector<RenderPassData> m_RenderPassDatas{};
	};
}
