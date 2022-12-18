#include "Pch.h"
#include "MaterialGraphEditor.h"

namespace Wuya
{
	/* ��ʾ/���ر༭�� */
	void MaterialGraphEditor::ShowOrHide()
	{
		m_IsShow = !m_IsShow;
	}

	/* �������UI */
	void MaterialGraphEditor::OnImGuiRenderer()
	{
		if (!m_IsShow)
			return;

		ImGui::Begin("Material Graph Editor");
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			const float icon_size = 24.0f;
			constexpr float cursor_offset = 10.0f;
			/* ���泡����ť */
			ImGui::SetCursorPosX(cursor_offset);
			static auto save_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/save.png", {});
			if (ImGui::ImageButton((ImTextureID)save_icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
			{
				SaveMaterial();
			}
			ImGui::PopStyleColor();

			ImGui::SameLine();
			if (ImGui::Button("Fit All"))
			{
				m_FitMode = GraphEditor::Fit_AllNodes;
			}

			ImGui::SameLine();
			if (ImGui::Button("Fit Selected"))
			{
				m_FitMode = GraphEditor::Fit_SelectedNodes;
			}

			ImGui::SameLine();
			if (ImGui::Button("Clear All"))
			{
				m_Delegate.ClearAll();
			}

			/* ��ʾ�ڵ�ͼ */
			GraphEditor::Show(m_Delegate, m_Options, m_ViewState, true, &m_FitMode);

			/* �հ״��Ҽ��������½� */
			if (m_Delegate.m_IsRightClick)
				ImGui::OpenPopup("CreateNodePopup");

			/* չ������ʱ����ʾ�ؼ� */
			if (ImGui::BeginPopup("CreateNodePopup"))
			{
				ImGui::PushItemWidth(120);

				if (ImGui::Button("Create Node"))
				{
					auto mouse_pos = Input::GetMousePos();
					m_Delegate.NodeArray.push_back({ "NewSimpleNode", 0, mouse_pos.x, mouse_pos.y, 200, 160, false });
					m_Delegate.m_IsRightClick = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	/* ������� */
	void MaterialGraphEditor::SaveMaterial()
	{
		// todo
	}

	template <typename T, std::size_t N>
	struct Array
	{
		T data[N];
		const size_t size() const { return N; }

		const T operator [] (size_t index) const { return data[index]; }
		operator T* () {
			T* p = new T[N];
			memcpy(p, data, sizeof(data));
			return p;
		}
	};

	template <typename T, typename ... U> Array(T, U...)->Array<T, 1 + sizeof...(U)>;

	/* ��ʼ���ڵ�ģ�� */
	std::vector<GraphEditor::Template> MaterialGraphEditor::MaterialGraphEditorDelegate::Templates = {
		{
		   IM_COL32(160, 160, 180, 255),
		   IM_COL32(100, 100, 140, 255),
		   IM_COL32(110, 110, 150, 255),
		   1,
		   Array{"MyInput"},
		   nullptr,
		   2,
		   Array{"MyOutput0", "MyOuput1"},
		   nullptr
	   },

	   {
		   IM_COL32(180, 160, 160, 255),
		   IM_COL32(140, 100, 100, 255),
		   IM_COL32(150, 110, 110, 255),
		   3,
		   nullptr,
		   Array{ IM_COL32(200,100,100,255), IM_COL32(100,200,100,255), IM_COL32(100,100,200,255) },
		   1,
		   Array{"MyOutput0"},
		   Array{ IM_COL32(200,200,200,255)}
	   }
	};

	/* ֧������ */
	bool MaterialGraphEditor::MaterialGraphEditorDelegate::AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to)
	{
		return true;
	}

	/* ѡ�нڵ� */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::SelectNode(GraphEditor::NodeIndex node_index, bool selected)
	{
		NodeArray[node_index].IsSelected = selected;
	}

	/* �ƶ�ѡ�нڵ� */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::MoveSelectedNodes(const ImVec2 delta)
	{
		for (auto& node : NodeArray)
		{
			if (!node.IsSelected)
				continue;

			node.X += delta.x;
			node.Y += delta.y;
		}
	}

	/* ���� */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::AddLink(GraphEditor::NodeIndex input_node_index, GraphEditor::SlotIndex input_slot_index, GraphEditor::NodeIndex output_node_index, GraphEditor::SlotIndex output_slot_index)
	{
		LinkArray.push_back({ input_node_index, input_slot_index, output_node_index, output_slot_index });
	}

	/* ɾ������ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::DelLink(GraphEditor::LinkIndex link_index)
	{
		LinkArray.erase(LinkArray.begin() + link_index);
	}

	/* �Զ������ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::CustomDraw(ImDrawList* draw_list, ImRect rectangle, GraphEditor::NodeIndex node_index)
	{
		draw_list->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
		draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
	}

	/* �Ҽ���Ӧ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::RightClick(GraphEditor::NodeIndex node_index, GraphEditor::SlotIndex slot_index_input, GraphEditor::SlotIndex slot_index_output)
	{
		m_IsRightClick = true;
	}

	/* ��ȡ�ڵ�ģ������ */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplateCount()
	{
		return Templates.size();
	}

	/* ��ȡָ���ڵ�ģ�� */
	const GraphEditor::Template MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplate(GraphEditor::TemplateIndex index)
	{
		ASSERT(index >= 0 && index < Templates.size());

		return Templates[index];
	}

	/* ��ȡ�ڵ����� */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetNodeCount()
	{
		return NodeArray.size();
	}

	/* ��ȡָ���ڵ� */
	const GraphEditor::Node MaterialGraphEditor::MaterialGraphEditorDelegate::GetNode(GraphEditor::NodeIndex index)
	{
		ASSERT(index >= 0 && index < NodeArray.size());

		auto& node = NodeArray[index];
		return { node.Name.c_str(), node.TemplateIndex, ImRect(node.X, node.Y, node.X + node.SizeX, node.Y + node.SizeY), node.IsSelected };
	}

	/* ��ȡ�������� */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetLinkCount()
	{
		return LinkArray.size();
	}

	/* ��ȡָ������ */
	const GraphEditor::Link MaterialGraphEditor::MaterialGraphEditorDelegate::GetLink(GraphEditor::LinkIndex index)
	{
		ASSERT(index >= 0 && index < LinkArray.size());

		return LinkArray[index];
	}

	/* ������нڵ������ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::ClearAll()
	{
		NodeArray.clear();
		LinkArray.clear();
	}
}
