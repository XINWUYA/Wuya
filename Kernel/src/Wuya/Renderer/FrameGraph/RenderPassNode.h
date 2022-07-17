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

	/* ����RenderPass��������ݣ�һ��RenderPass���ܰ����������� */
	struct RenderPassData
	{
		/* ������ */
		std::string Name{};
		/* ���� */
		FrameGraphPassInfo::Descriptor Descriptor{};
		/* �������ݽڵ� */
		RenderResourceNode* IncomingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* ������ݽڵ� */
		RenderResourceNode* OutgoingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
	};

	/* RenderPassNode�࣬��ΪFrameGraph������ϵͼ�е�һ���ڵ� */
	class RenderPassNode : public DependencyGraph::Node
	{
	public:
		RenderPassNode(FrameGraph& frame_graph, const std::string& name, IFrameGraphPass* frame_graph_pass);
		virtual ~RenderPassNode() = default;

		/* ��ȡPass�� */
		const std::string& GetName() const { return Name; }
		/* ��ȡ��Դ */
		std::vector<IResource*>& GetResourcesNeedPrepared() { return m_ResourcesNeedPrepared; }
		const std::vector<IResource*>& GetResourcesNeedPrepared() const { return m_ResourcesNeedPrepared; }
		std::vector<IResource*>& GetResourcesNeedDestroy() { return m_ResourcesNeedDestroy; }
		const std::vector<IResource*>& GetResourcesNeedDestroy() const { return m_ResourcesNeedDestroy; }

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
		IFrameGraphPass* m_OwnerFrameGraphPass;
		/* ��ע�����ԴHandles */
		std::unordered_set<uint16_t> m_RegisteredHandleIndices{};
		/* ��Ҫ׼������Դ */
		std::vector<IResource*> m_ResourcesNeedPrepared{};
		/* ��Ҫ�ͷŵ���Դ */
		std::vector<IResource*> m_ResourcesNeedDestroy{};
		/*��ǰPass���� */
		std::vector<RenderPassData> m_RenderPassDatas{};
	};
}
