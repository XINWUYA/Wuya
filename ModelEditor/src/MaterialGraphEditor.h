#include "GraphEditor.h"
#include "MaterialGraphNodes.h"

namespace Wuya
{
	/* 材质图编辑器类
	 * 用于修改材质的实际实现
	 */
	class MaterialGraphEditor
	{
	public:
		MaterialGraphEditor();
		~MaterialGraphEditor() = default;

		/* 显示/隐藏编辑器 */
		void ShowOrHide();

		/* 绘制相关UI */
		void OnImGuiRenderer();

		/* 序列化 */
		void Serializer(const std::string& path);
		/* 反序列化*/
		void Deserializer(const std::string& path);

	private:
		/* 保存材质 */
		void SaveMaterial();

		/* 继承ImGuizmo中的GraphEditor::Delegate, 用于自定义节点的行为 */
		struct MaterialGraphEditorDelegate : public GraphEditor::Delegate
		{
			/* 构造时执行 */
			MaterialGraphEditorDelegate();
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
			/* 创建一个指定类型的Node */
			MaterialGraphNode CreateNode(MaterialGraphNodeType node_type, const ImVec2& ScreenPos = ImVec2(0,0));
			/* 清楚所有节点和连线 */
			void ClearAll();
			/* 全选节点 */
			void SelectAllNodes();
			/* 删除选中节点 */
			void DeleteSelectedNodes();

			/* 右键空白处 */
			bool m_IsRightClickEmpty{ false };
			/* 节点列表 */
			std::vector<MaterialGraphNode> NodeArray{};
			/* 连线列表 */
			std::vector<GraphEditor::Link> LinkArray{};
			/* Registry */
			entt::registry Registry{};
		};

		/* 是否显示 */
		bool m_IsShow{ false };
		/* 路径 */
		std::string m_Path{};
		/* 显示选项 */
		GraphEditor::Options m_Options{};
		GraphEditor::ViewState m_ViewState{};
		GraphEditor::FitOnScreen m_FitMode{ GraphEditor::Fit_AllNodes };
		/* 自定义的委托 */
		MaterialGraphEditorDelegate m_Delegate{};
	};
}
