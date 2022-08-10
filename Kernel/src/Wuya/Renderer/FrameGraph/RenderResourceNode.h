#pragma once
#include "DependencyGraph.h"
#include "FrameGraphResourceHandle.h"

namespace Wuya
{
	class FrameGraph;
	class RenderPassNode;
	/* */
	class RenderResourceNode : public DependencyGraph::Node
	{
	public:
		RenderResourceNode(FrameGraph& frame_graph, FrameGraphResourceHandle handle);
		~RenderResourceNode() override;

		/* 获取当前资源的Handle */
		const FrameGraphResourceHandle& GetResourceHandle() const { return m_ResourceHandle; }
		/* 获取资源名 */
		const std::string& GetName() const { return Name; }

		/* 增加使用节点 */
		void SetIncomingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_pIncomingConnection = connection; }
		void AddOutgoingConnection(const SharedPtr<DependencyGraph::Connection>& connection) { m_OutgoingConnections.emplace_back(connection); }
		/* 获取资源的作为输入/输出的使用情况 */
		bool HasOutgoingConnection() const { return !m_OutgoingConnections.empty(); }
		bool HasIncomingConnection() const { return m_pIncomingConnection != nullptr; }
		/* 从节点的角度出发，获取与资源之间的连线 */
		const SharedPtr<DependencyGraph::Connection>& GetOutgoingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;
		const SharedPtr<DependencyGraph::Connection>& GetIncomingConnectionOfPassNode(const SharedPtr<RenderPassNode>& node) const;
		/* 更新资源的Usage */
		void UpdateResourceUsage();

		/* 销毁 */
		void Destroy();

	private:
		/* 输出可视化设置 */
		std::string Graphvizify() const override;
		std::string GraphvizifyConnectionColor() const override;

		/* 资源句柄，一个句柄对应FrameGraph中的一个资源 */
		FrameGraphResourceHandle m_ResourceHandle;
		/* 所属的FrameGraph */
		FrameGraph& m_FrameGraph;
		/* 资源的使用情况：一个资源可能被多个Pass使用，但最多只支持被1个Pass来写入 */
		std::vector<SharedPtr<DependencyGraph::Connection>> m_OutgoingConnections{};
		SharedPtr<DependencyGraph::Connection> m_pIncomingConnection{ nullptr };
	};
}
