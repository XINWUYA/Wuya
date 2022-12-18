#include "GraphEditor.h"

namespace Wuya
{
	/* 材质图编辑器类
	 * 用于修改材质的实际实现
	 */
	class MaterialGraphEditor
	{
	public:
		MaterialGraphEditor() = default;
		~MaterialGraphEditor() = default;

		/* 显示/隐藏编辑器 */
		void ShowOrHide();

		/* 绘制相关UI */
		void OnImGuiRenderer();

	private:
		/* 保存材质 */
		void SaveMaterial();

		/* 继承ImGuizmo中的GraphEditor::Delegate, 用于自定义节点的行为 */
		struct MaterialGraphEditorDelegate : public GraphEditor::Delegate
		{
			/* 支持连线 */
			bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override;
			/* 选中节点 */
			void SelectNode(GraphEditor::NodeIndex node_index, bool selected) override;
			/* 移动选中节点 */
			void MoveSelectedNodes(const ImVec2 delta) override;
			/* 连线 */
			void AddLink(GraphEditor::NodeIndex input_node_index, GraphEditor::SlotIndex input_slot_index, GraphEditor::NodeIndex output_node_index, GraphEditor::SlotIndex output_slot_index) override;
			/* 删除连线 */
			void DelLink(GraphEditor::LinkIndex link_index) override;
			/* 自定义绘制 */
			void CustomDraw(ImDrawList* draw_list, ImRect rectangle, GraphEditor::NodeIndex node_index) override;
			/* 右键响应 */
			void RightClick(GraphEditor::NodeIndex node_index, GraphEditor::SlotIndex slot_index_input, GraphEditor::SlotIndex slot_index_output) override;
			/* 获取节点模板数量 */
			const size_t GetTemplateCount() override;
			/* 获取指定节点模板 */
			const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override;
			/* 获取节点数量 */
			const size_t GetNodeCount() override;
			/* 获取指定节点 */
			const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override;
			/* 获取连线数量 */
			const size_t GetLinkCount() override;
			/* 获取指定连线 */
			const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override;
			/* 清楚所有节点和连线 */
			void ClearAll();

			/* 节点属性 */
			struct Node
			{
				std::string Name;
				GraphEditor::TemplateIndex TemplateIndex;
				float X, Y;
				float SizeX = 200.0f, SizeY = 200.0f;
				bool IsSelected;
			};
			/* 响应右键 */
			bool m_IsRightClick{ false };
			/* 节点列表 */
			std::vector<Node> NodeArray{};
			/* 连线列表 */
			std::vector<GraphEditor::Link> LinkArray{};
			/* 节点模板 */
			static std::vector<GraphEditor::Template> Templates;
		};

		/* 是否显示 */
		bool m_IsShow{ false };
		/* 显示选项 */
		GraphEditor::Options m_Options{};
		GraphEditor::ViewState m_ViewState{};
		GraphEditor::FitOnScreen m_FitMode{ GraphEditor::Fit_None };
		/* 自定义的委托 */
		MaterialGraphEditorDelegate m_Delegate{};
	};
}
