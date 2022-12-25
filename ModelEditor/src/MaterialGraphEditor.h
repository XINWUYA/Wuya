#include "GraphEditor.h"
#include "MaterialGraphNodes.h"

namespace Wuya
{
	/* ����ͼ�༭����
	 * �����޸Ĳ��ʵ�ʵ��ʵ��
	 */
	class MaterialGraphEditor
	{
	public:
		MaterialGraphEditor();
		~MaterialGraphEditor() = default;

		/* ��ʾ/���ر༭�� */
		void ShowOrHide();

		/* �������UI */
		void OnImGuiRenderer();

		/* ���л� */
		void Serializer(const std::string& path);
		/* �����л�*/
		void Deserializer(const std::string& path);

	private:
		/* ������� */
		void SaveMaterial();

		/* �̳�ImGuizmo�е�GraphEditor::Delegate, �����Զ���ڵ����Ϊ */
		struct MaterialGraphEditorDelegate : public GraphEditor::Delegate
		{
			/* ����ʱִ�� */
			MaterialGraphEditorDelegate();
			/* ֧������ */
			bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override;
			/* ѡ�нڵ� */
			void SelectNode(GraphEditor::NodeIndex node_index, bool selected) override;
			/* �ƶ�ѡ�нڵ� */
			void MoveSelectedNodes(const ImVec2 delta) override;
			/* ���� */
			void AddLink(GraphEditor::NodeIndex input_node_index, GraphEditor::SlotIndex input_slot_index, GraphEditor::NodeIndex output_node_index, GraphEditor::SlotIndex output_slot_index) override;
			/* ɾ������ */
			void DelLink(GraphEditor::LinkIndex link_index) override;
			/* �Զ������ */
			void CustomDraw(ImDrawList* draw_list, ImRect rectangle, GraphEditor::NodeIndex node_index) override;
			/* �Ҽ���Ӧ */
			void RightClick(GraphEditor::NodeIndex node_index, GraphEditor::SlotIndex slot_index_input, GraphEditor::SlotIndex slot_index_output) override;
			/* ��ȡ�ڵ�ģ������ */
			const size_t GetTemplateCount() override;
			/* ��ȡָ���ڵ�ģ�� */
			const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override;
			/* ��ȡ�ڵ����� */
			const size_t GetNodeCount() override;
			/* ��ȡָ���ڵ� */
			const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override;
			/* ��ȡ�������� */
			const size_t GetLinkCount() override;
			/* ��ȡָ������ */
			const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override;
			/* ����һ��ָ�����͵�Node */
			MaterialGraphNode CreateNode(MaterialGraphNodeType node_type, const ImVec2& ScreenPos = ImVec2(0,0));
			/* ������нڵ������ */
			void ClearAll();
			/* ȫѡ�ڵ� */
			void SelectAllNodes();
			/* ɾ��ѡ�нڵ� */
			void DeleteSelectedNodes();

			/* �Ҽ��հ״� */
			bool m_IsRightClickEmpty{ false };
			/* �ڵ��б� */
			std::vector<MaterialGraphNode> NodeArray{};
			/* �����б� */
			std::vector<GraphEditor::Link> LinkArray{};
			/* Registry */
			entt::registry Registry{};
		};

		/* �Ƿ���ʾ */
		bool m_IsShow{ false };
		/* ·�� */
		std::string m_Path{};
		/* ��ʾѡ�� */
		GraphEditor::Options m_Options{};
		GraphEditor::ViewState m_ViewState{};
		GraphEditor::FitOnScreen m_FitMode{ GraphEditor::Fit_AllNodes };
		/* �Զ����ί�� */
		MaterialGraphEditorDelegate m_Delegate{};
	};
}
