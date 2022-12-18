#include "GraphEditor.h"

namespace Wuya
{
	/* ����ͼ�༭����
	 * �����޸Ĳ��ʵ�ʵ��ʵ��
	 */
	class MaterialGraphEditor
	{
	public:
		MaterialGraphEditor() = default;
		~MaterialGraphEditor() = default;

		/* ��ʾ/���ر༭�� */
		void ShowOrHide();

		/* �������UI */
		void OnImGuiRenderer();

	private:
		/* ������� */
		void SaveMaterial();

		/* �̳�ImGuizmo�е�GraphEditor::Delegate, �����Զ���ڵ����Ϊ */
		struct MaterialGraphEditorDelegate : public GraphEditor::Delegate
		{
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
			/* ������нڵ������ */
			void ClearAll();

			/* �ڵ����� */
			struct Node
			{
				std::string Name;
				GraphEditor::TemplateIndex TemplateIndex;
				float X, Y;
				float SizeX = 200.0f, SizeY = 200.0f;
				bool IsSelected;
			};
			/* ��Ӧ�Ҽ� */
			bool m_IsRightClick{ false };
			/* �ڵ��б� */
			std::vector<Node> NodeArray{};
			/* �����б� */
			std::vector<GraphEditor::Link> LinkArray{};
			/* �ڵ�ģ�� */
			static std::vector<GraphEditor::Template> Templates;
		};

		/* �Ƿ���ʾ */
		bool m_IsShow{ false };
		/* ��ʾѡ�� */
		GraphEditor::Options m_Options{};
		GraphEditor::ViewState m_ViewState{};
		GraphEditor::FitOnScreen m_FitMode{ GraphEditor::Fit_None };
		/* �Զ����ί�� */
		MaterialGraphEditorDelegate m_Delegate{};
	};
}
