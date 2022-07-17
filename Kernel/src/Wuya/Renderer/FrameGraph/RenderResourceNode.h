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
		~RenderResourceNode();

		/* 获取当前资源的Handle */
		const FrameGraphResourceHandle& GetResourceHandle() const { return m_ResourceHandle; }
		/* 获取资源名 */
		const std::string& GetName() const { return Name; }

		/* 增加使用节点 */
		void SetIncomingConnection(DependencyGraph::Connection* connection) { m_pIncomingConnection = connection; }
		void AddOutgoingConnection(DependencyGraph::Connection* connection) { m_OutgoingConnections.emplace_back(connection); }
		/* 获取资源的使用情况 */
		bool HasOutgoingConnection() const { return !m_OutgoingConnections.empty(); }
		bool HasIncomingConnection() const { return m_pIncomingConnection != nullptr; }
		/* 从节点的角度出发，获取与资源之间的连线 */
		DependencyGraph::Connection* GetOutgoingConnectionOfPassNode(const RenderPassNode* node) const;
		DependencyGraph::Connection* GetIncomingConnectionOfPassNode(const RenderPassNode* node) const;


	private:
		/* 输出可视化设置 */
		std::string Graphvizify() const override;
		std::string GraphvizifyConnectionColor() const override;

		/* 资源句柄，一个句柄对应FrameGraph中的一个资源 */
		FrameGraphResourceHandle m_ResourceHandle;
		/* 所属的FrameGraph */
		FrameGraph& m_FrameGraph;
		/* 资源的使用情况：一个资源可能被多个Pass使用，但最多只能被1个Pass来写入 */
		std::vector<DependencyGraph::Connection*> m_OutgoingConnections{};
		DependencyGraph::Connection* m_pIncomingConnection{ nullptr };
	};
}
