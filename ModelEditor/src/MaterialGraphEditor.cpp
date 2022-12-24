#include "Pch.h"
#include "MaterialGraphEditor.h"

#include "PackedUIFuncs.h"

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	MaterialGraphEditor::MaterialGraphEditor()
	{
		/* Ĭ����ʾ���ж˿��� */
		m_Options.mDrawIONameOnHover = false;
		m_Options.mLineThickness = 2.0f; /* ���ߴ�ϸ */
		m_Options.mNodeSlotRadius = 6.0f; /* ���ӵ�İ뾶 */
	}

	/* ��ʾ/���ر༭�� */
	void MaterialGraphEditor::ShowOrHide()
	{
		m_IsShow = !m_IsShow;
	}

	/* �������UI */
	void MaterialGraphEditor::OnImGuiRenderer()
	{
		PROFILE_FUNCTION();

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
			if (m_Delegate.m_IsRightClickEmpty)
				ImGui::OpenPopup("CreateNodePopup");

			/* ��ʾ */
			auto ShowCreateNodeButton = [&](const char* label, MaterialGraphNodeType node_type)
			{
				ImGui::PushItemWidth(160);
				if (ImGui::Button(label, ImVec2(160, 24)))
				{
					const auto& mouse_pos = Input::GetMousePos();
					m_Delegate.CreateNode(node_type, ImVec2(mouse_pos.x, mouse_pos.y));
					m_Delegate.m_IsRightClickEmpty = false;
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopItemWidth();
			};

			/* չ������ʱ����ʾ�ؼ� */
			if (ImGui::BeginPopup("CreateNodePopup"))
			{
				ShowCreateNodeButton("Texture2D", MaterialGraphNodeType::Texture2D);
				ShowCreateNodeButton("SamplerState", MaterialGraphNodeType::SamplerState);
				ShowCreateNodeButton("Float", MaterialGraphNodeType::Float);
				ShowCreateNodeButton("Float2", MaterialGraphNodeType::Float2);
				ShowCreateNodeButton("Float3", MaterialGraphNodeType::Float3);
				ShowCreateNodeButton("Float4", MaterialGraphNodeType::Float4);
				ShowCreateNodeButton("Color", MaterialGraphNodeType::Color);
				ShowCreateNodeButton("Operator", MaterialGraphNodeType::Operator);

				ImGui::EndPopup();
			}
		}
		ImGui::End();

		/* ��ʾѡ�нڵ�����ԣ����ڱ༭ */
		ImGui::Begin("Properties");
		{
			PROFILE_SCOPE("Show Material Graph Node's Properties")

			/* ��ʹ��ѡ��Ҳֻ��ʾ��һ���ڵ������ */
			int selected_node_idx = -1;
			for (size_t i = 0; i < m_Delegate.GetNodeCount(); ++i)
			{
				auto& node = m_Delegate.NodeArray[i];
				if (node.IsSelected)
				{
					selected_node_idx = static_cast<int>(i);
					break;
				}
			}

			/* ����ѡ�нڵ� */
			if (selected_node_idx > 0)
			{
				auto& node = m_Delegate.NodeArray[selected_node_idx];
				switch (node.NodeType)
				{
				case MaterialGraphNodeType::PBRMaterial: break;
				case MaterialGraphNodeType::Texture2D:
					{
						/* ��ʾ��� */
						auto& component = m_Delegate.Registry.get_or_emplace<MGTexture2DComponent>(node.EntityHandle);
						PackedUIFuncs::DrawTextureUI("Texture", component.Texture, component.TilingFactor);
					}
					break;
				case MaterialGraphNodeType::SamplerState:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGSamplerStateComponent>(node.EntityHandle);

						int warp_mode_idx = static_cast<int>(component.WrapMode);
						PackedUIFuncs::DrawComboUI("WrapType", { "ClampToEdge", "Repeat", "MirroredRepeat"}, warp_mode_idx);
						component.WrapMode = static_cast<SamplerWrapMode>(warp_mode_idx);

						int min_filter_index = static_cast<int>(component.MinFilter);
						PackedUIFuncs::DrawComboUI("MinFilter", { "Nearest", "Linear", "NearestMipmapNearest", "LinearMipmapNearest", "NearestMipmapLinear", "LinearMipmapLinear"}, min_filter_index);
						component.MinFilter = static_cast<SamplerMinFilter>(min_filter_index);

						int mag_filter_index = static_cast<int>(component.MagFilter);
						PackedUIFuncs::DrawComboUI("MagFilter", { "Nearest", "Linear"}, mag_filter_index);
						component.MagFilter = static_cast<SamplerMagFilter>(mag_filter_index);
					}
					break;
				case MaterialGraphNodeType::Float:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGFloatComponent>(node.EntityHandle);
						PackedUIFuncs::DrawDragFloatUI("Float", component.Value);
					}
					break;
				case MaterialGraphNodeType::Float2:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGFloat2Component>(node.EntityHandle);
						PackedUIFuncs::DrawDragFloat2UI("Float2", component.Value);
					}
					break;
				case MaterialGraphNodeType::Float3:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGFloat3Component>(node.EntityHandle);
						PackedUIFuncs::DrawDragFloat3UI("Float3", component.Value);
					}
					break;
				case MaterialGraphNodeType::Float4:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGFloat4Component>(node.EntityHandle);
						PackedUIFuncs::DrawDragFloat4UI("Float4", component.Value);
					}
					break;
				case MaterialGraphNodeType::Color:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGColorComponent>(node.EntityHandle);
						PackedUIFuncs::DrawColorUI("Color", component.Color);
					}
					break;
				case MaterialGraphNodeType::Operator:
					{
						auto& component = m_Delegate.Registry.get_or_emplace<MGOperatorComponent>(node.EntityHandle);
						int operator_type_idx = static_cast<int>(component.OperatorType);
						PackedUIFuncs::DrawComboUI("OperatorType", { "Add", "Subtract", "Multiply", "Divide" }, operator_type_idx);
						component.OperatorType = static_cast<MGNodeOperatorType>(operator_type_idx);

						/* �ݲ�֧�ֶ�Ԫ���������鷳̫�� */
						//int element_cnt_index = component.ElementCnt - 2;
						//PackedUIFuncs::DrawComboUI("ElementCnt", {"2", "3", "4"}, element_cnt_index);
						///* ��Ԫ�����������ı�ʱ */
						//if (element_cnt_index != component.ElementCnt - 2)
						//{
						//	component.ElementCnt = element_cnt_index + 2;
						//	node.TemplateIndex = TemplateIndex_Operator2 + element_cnt_index;
						//	node.Height = 120 + 40 * element_cnt_index;
						//}
					}
					break;
				}
			}
		}
		ImGui::End();
	}

	/* ������� */
	void MaterialGraphEditor::SaveMaterial()
	{
		// todo
	}

	/* ����ʱ��Ĭ������һ��PBR�ڵ� */
	MaterialGraphEditor::MaterialGraphEditorDelegate::MaterialGraphEditorDelegate()
	{
		CreateNode(MaterialGraphNodeType::PBRMaterial);
	}

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

			node.ScreenPosX += delta.x;
			node.ScreenPosY += delta.y;
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
		PROFILE_FUNCTION();

		//draw_list->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
		auto& node = NodeArray[node_index];
		switch (node.NodeType)
		{
		case MaterialGraphNodeType::PBRMaterial: break;
		case MaterialGraphNodeType::Texture2D:
			{
				auto& component = Registry.get_or_emplace<MGTexture2DComponent>(node.EntityHandle);
				draw_list->AddImage((ImTextureID)component.Texture->GetTextureID(), ImVec2(rectangle.Min.x + 5, rectangle.Min.y + 5), ImVec2(rectangle.Max.x - 5, rectangle.Max.y - 5), ImVec2(0, 1), ImVec2(1, 0), 0xFFFFFFFF);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						const std::filesystem::path texture_path = g_AssetPath / path;
						component.Texture = TextureAssetManager::Instance().GetOrCreateTexture(texture_path.generic_string(), {});
					}
					ImGui::EndDragDropTarget();
				}

				/* ����ӹ�ʱ��ʾͼƬ·�� */
				if (ImGui::IsItemHovered())
					draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), component.Texture->GetPath().c_str());
			}
			break;
		case MaterialGraphNodeType::SamplerState:
			{
				const auto& component = Registry.get_or_emplace<MGSamplerStateComponent>(node.EntityHandle);

			}
			break;
		case MaterialGraphNodeType::Float:
			{
				const auto& component = Registry.get_or_emplace<MGFloatComponent>(node.EntityHandle);
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), ToString(component.Value, 3).c_str());
			}
			break;
		case MaterialGraphNodeType::Float2:
			{
				const auto& component = Registry.get_or_emplace<MGFloat2Component>(node.EntityHandle);
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), ToString(component.Value, 3).c_str());
			}
			break;
		case MaterialGraphNodeType::Float3:
			{
				const auto& component = Registry.get_or_emplace<MGFloat3Component>(node.EntityHandle);
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), ToString(component.Value, 3).c_str());
			}
			break;
		case MaterialGraphNodeType::Float4:
			{
				const auto& component = Registry.get_or_emplace<MGFloat4Component>(node.EntityHandle);
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), ToString(component.Value, 3).c_str());
			}
			break;
		case MaterialGraphNodeType::Color:
			{
				const auto& component = Registry.get_or_emplace<MGColorComponent>(node.EntityHandle);
				const auto color = ImColor(component.Color.r, component.Color.g, component.Color.b, component.Color.a);
				draw_list->AddRectFilled(rectangle.Min, rectangle.Max, color);
			}
			break;
		case MaterialGraphNodeType::Operator:
			{
				const auto& component = Registry.get_or_emplace<MGOperatorComponent>(node.EntityHandle);
				std::string type;
				switch (component.OperatorType)
				{
				case MGNodeOperatorType::Add:
					type = "Add";
					break;
				case MGNodeOperatorType::Subtract: 
					type = "Subtract";
					break;
				case MGNodeOperatorType::Multiply: 
					type = "Multiply";
					break;
				case MGNodeOperatorType::Divide: 
					type = "Divide";
					break;
				}
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), type.c_str());
			}
			break;
		}
	}

	/* �Ҽ���Ӧ��ֻ���Ҽ����ڵ�Ķ˿�ʱ����Ч */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::RightClick(GraphEditor::NodeIndex node_index, GraphEditor::SlotIndex slot_index_input, GraphEditor::SlotIndex slot_index_output)
	{
		if (node_index != -1)
		{
			auto& node = NodeArray[node_index];

			return;
		}

		m_IsRightClickEmpty = true;
	}

	/* ��ȡ�ڵ�ģ������ */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplateCount()
	{
		return s_Templates.size();
	}

	/* ��ȡָ���ڵ�ģ�� */
	const GraphEditor::Template MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplate(GraphEditor::TemplateIndex index)
	{
		ASSERT(index < s_Templates.size());

		return s_Templates[index];
	}

	/* ��ȡ�ڵ����� */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetNodeCount()
	{
		return NodeArray.size();
	}

	/* ��ȡָ���ڵ� */
	const GraphEditor::Node MaterialGraphEditor::MaterialGraphEditorDelegate::GetNode(GraphEditor::NodeIndex index)
	{
		ASSERT(index < NodeArray.size());

		const auto& node = NodeArray[index];
		return {
			node.Name.c_str(),
			node.TemplateIndex,
			ImRect(node.ScreenPosX, node.ScreenPosY, node.ScreenPosX + node.Width, node.ScreenPosY + node.Height),
			node.IsSelected
		};
	}

	/* ��ȡ�������� */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetLinkCount()
	{
		return LinkArray.size();
	}

	/* ��ȡָ������ */
	const GraphEditor::Link MaterialGraphEditor::MaterialGraphEditorDelegate::GetLink(GraphEditor::LinkIndex index)
	{
		ASSERT(index < LinkArray.size());

		return LinkArray[index];
	}

	/* ����һ��ָ�����͵�Node */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::CreateNode(MaterialGraphNodeType node_type, const ImVec2& ScreenPos)
	{
		PROFILE_FUNCTION();

		MaterialGraphNode node_inst;
		node_inst.NodeType = node_type;
		node_inst.ScreenPosX = ScreenPos.x;
		node_inst.ScreenPosY = ScreenPos.y;

		switch (node_inst.NodeType)
		{
		case MaterialGraphNodeType::PBRMaterial:
			{
				node_inst.Name = "PBRMaterial";
				node_inst.TemplateIndex = TemplateIndex_PBRMaterial;
				node_inst.Width = 160;
				node_inst.Height = 800;
				node_inst.IsSelected = true; // Ĭ�ϱ�ѡ�У������״���ʾʱ��Ӧ����Ļ
			}
			break;
		case MaterialGraphNodeType::Texture2D:
			{
				node_inst.Name = "Texture2D";
				node_inst.TemplateIndex = TemplateIndex_Texture2D;
				node_inst.Width = 160;
				node_inst.Height = 180;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGTexture2DComponent>(node_inst.EntityHandle);
				component.Texture = TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/default_texture.png", {});
			}
			break;
		case MaterialGraphNodeType::SamplerState:
			{
				node_inst.Name = "SamplerState";
				node_inst.TemplateIndex = TemplateIndex_SamplerState;
				node_inst.Width = 160;
				node_inst.Height = 300;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGSamplerStateComponent>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Float:
			{
				node_inst.Name = "Float";
				node_inst.TemplateIndex = TemplateIndex_Float;
				node_inst.Width = 160;
				node_inst.Height = 70;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGFloatComponent>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Float2: 
			{
				node_inst.Name = "Float2";
				node_inst.TemplateIndex = TemplateIndex_Float2;
				node_inst.Width = 160;
				node_inst.Height = 70;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGFloat2Component>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Float3:
			{
				node_inst.Name = "Float3";
				node_inst.TemplateIndex = TemplateIndex_Float3;
				node_inst.Width = 200;
				node_inst.Height = 70;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGFloat3Component>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Float4:
			{
				node_inst.Name = "Float4";
				node_inst.TemplateIndex = TemplateIndex_Float4;
				node_inst.Width = 240;
				node_inst.Height = 70;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGFloat4Component>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Color:
			{
				node_inst.Name = "Color";
				node_inst.TemplateIndex = TemplateIndex_Color;
				node_inst.Width = 160;
				node_inst.Height = 70;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGColorComponent>(node_inst.EntityHandle);
			}
			break;
		case MaterialGraphNodeType::Operator:
			{
				node_inst.Name = "Operator";
				node_inst.TemplateIndex = TemplateIndex_Operator2;
				node_inst.Width = 160;
				node_inst.Height = 120;
				node_inst.IsSelected = false;

				/* ���������� */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGOperatorComponent>(node_inst.EntityHandle);
			}
			break;
		}

		NodeArray.emplace_back(node_inst);
	}

	/* ������нڵ������ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::ClearAll()
	{
		NodeArray.clear();
		LinkArray.clear();
	}
}
