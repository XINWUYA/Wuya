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

	/* 包含RenderPass所需的数据，一个RenderPass可能包含多组数据 */
	struct RenderPassData
	{
		/* 数据名 */
		std::string Name{};
		/* 描述 */
		FrameGraphPassInfo::Descriptor Descriptor{};
		/* 输入数据节点 */
		SharedPtr<RenderResourceNode> IncomingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* 输出数据节点 */
		SharedPtr<RenderResourceNode> OutgoingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* 收集有效的Attachments */
		FrameGraphResourceHandleTyped<FrameGraphTexture> ValidAttachments[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* RenderBuffer的使用情况 */
		RenderBufferUsage RenderBufferUsage{ RenderBufferUsage::None };
		/* FrameBuffer */
		SharedPtr<FrameBuffer> FrameBuffer{ nullptr };
	};

	/* RenderPassNode类，作为FrameGraph依赖关系图中的一个节点 */
	class RenderPassNode : public DependencyGraph::Node, public std::enable_shared_from_this<RenderPassNode>
	{
	public:
		RenderPassNode(FrameGraph& frame_graph, const std::string& name, const SharedPtr<IFrameGraphPass>& frame_graph_pass);
		virtual ~RenderPassNode() override;

		/* 获取Pass名 */
		const std::string& GetName() const { return Name; }
		/* 获取资源 */
		std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() { return m_ResourcesNeedPrepared; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedPrepared() const { return m_ResourcesNeedPrepared; }
		std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() { return m_ResourcesNeedDestroy; }
		const std::vector<SharedPtr<IResource>>& GetResourcesNeedDestroy() const { return m_ResourcesNeedDestroy; }

		/* 注册资源Handle */
		void RegisterResourceHandle(FrameGraphResourceHandle handle);
		/* 判断资源Handle是否被注册 */
		bool IsResourceHandleRegistered(FrameGraphResourceHandle handle) const;

		/* 创建RenderPassData */
		uint32_t CreateRenderPassData(const std::string& name, const FrameGraphPassInfo::Descriptor& desc);
		/* 获取RenderPassData */
		const RenderPassData* GetRenderPassData(uint32_t idx) const;
		/* 构建阶段 */
		void Build();
		/* 执行阶段 */
		void Execute(const FrameGraphResources& resources);
		/* 销毁 */
		void Destroy();

	private:
		/* 输出可视化设置 */
		std::string Graphvizify() const override;
		std::string GraphvizifyConnectionColor() const override;

		/* 所属FrameGraph */
		FrameGraph& m_OwnerFrameGraph;
		/* 所属的FrameGraphPass */
		SharedPtr<IFrameGraphPass> m_OwnerFrameGraphPass{ nullptr };
		/* 已注册的资源Handles */
		std::unordered_set<uint16_t> m_RegisteredHandleIndices{};
		/* 需要准备的资源，仅持有指针，内存由FrameGraph统一管理 */
		std::vector<SharedPtr<IResource>> m_ResourcesNeedPrepared{};
		/* 需要释放的资源，仅持有指针，内存由FrameGraph统一管理 */
		std::vector<SharedPtr<IResource>> m_ResourcesNeedDestroy{};
		/*当前Pass数据 */
		std::vector<RenderPassData> m_RenderPassDatas{};
	};
}
