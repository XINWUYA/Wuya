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

	/* 包含RenderPass所需的数据，一个RenderPass可能包含多组数据 */
	struct RenderPassData
	{
		/* 数据名 */
		std::string Name{};
		/* 描述 */
		FrameGraphPassInfo::Descriptor Descriptor{};
		/* 输入数据节点 */
		RenderResourceNode* IncomingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
		/* 输出数据节点 */
		RenderResourceNode* OutgoingResourceNodes[FrameGraphPassInfo::MAX_ATTACHMENT_NUM] = {};
	};

	/* RenderPassNode类，作为FrameGraph依赖关系图中的一个节点 */
	class RenderPassNode : public DependencyGraph::Node
	{
	public:
		RenderPassNode(FrameGraph& frame_graph, const std::string& name, IFrameGraphPass* frame_graph_pass);
		virtual ~RenderPassNode() = default;

		/* 获取Pass名 */
		const std::string& GetName() const { return Name; }
		/* 获取资源 */
		std::vector<IResource*>& GetResourcesNeedPrepared() { return m_ResourcesNeedPrepared; }
		const std::vector<IResource*>& GetResourcesNeedPrepared() const { return m_ResourcesNeedPrepared; }
		std::vector<IResource*>& GetResourcesNeedDestroy() { return m_ResourcesNeedDestroy; }
		const std::vector<IResource*>& GetResourcesNeedDestroy() const { return m_ResourcesNeedDestroy; }

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
		IFrameGraphPass* m_OwnerFrameGraphPass;
		/* 已注册的资源Handles */
		std::unordered_set<uint16_t> m_RegisteredHandleIndices{};
		/* 需要准备的资源 */
		std::vector<IResource*> m_ResourcesNeedPrepared{};
		/* 需要释放的资源 */
		std::vector<IResource*> m_ResourcesNeedDestroy{};
		/*当前Pass数据 */
		std::vector<RenderPassData> m_RenderPassDatas{};
	};
}
