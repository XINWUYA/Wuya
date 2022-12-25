#include "Pch.h"
#include "MaterialGraphEditor.h"
#include "PackedUIFuncs.h"
#include <tinyxml2.h>

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

		ImGuiIO& io = ImGui::GetIO();

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
				if (m_Path.empty())
				{
					m_Path = FileDialog::SaveFile("MtlGraph(*.mtlgraph)\0*.mtlgraph\0");
				}
				Serializer(m_Path);
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
					const auto& mouse_pos = io.MousePos;
					m_Delegate.NodeArray.emplace_back(m_Delegate.CreateNode(node_type, ImVec2(mouse_pos.x, mouse_pos.y)));
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
						PackedUIFuncs::DrawComboUI("WrapType", GetEnumNames<SamplerWrapMode>(), warp_mode_idx);
						component.WrapMode = static_cast<SamplerWrapMode>(warp_mode_idx);

						int min_filter_index = static_cast<int>(component.MinFilter);
						PackedUIFuncs::DrawComboUI("MinFilter", GetEnumNames<SamplerMinFilter>(), min_filter_index);
						component.MinFilter = static_cast<SamplerMinFilter>(min_filter_index);

						int mag_filter_index = static_cast<int>(component.MagFilter);
						PackedUIFuncs::DrawComboUI("MagFilter", GetEnumNames<SamplerMagFilter>(), mag_filter_index);
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
						PackedUIFuncs::DrawComboUI("OperatorType", GetEnumNames<MGNodeOperatorType>(), operator_type_idx);
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

		/* ImGui�¼���Ӧ */
		if (io.KeysDown[Key::Delete])
			m_Delegate.DeleteSelectedNodes();

		if (io.KeyCtrl)
		{
			if (io.KeysDown[Key::A]) /* Ctrl + A: ȫѡ���нڵ� */
				m_Delegate.SelectAllNodes();
		}
	}

	/* ���л� */
	void MaterialGraphEditor::Serializer(const std::string& path)
	{
		m_Path = path;

		ASSERT(!m_Path.empty());

		/* д�����ͼ��Ϣ */
		auto* out_mtl_graph_file = new tinyxml2::XMLDocument();
		out_mtl_graph_file->InsertEndChild(out_mtl_graph_file->NewDeclaration());
		auto* mtl_graph_root = out_mtl_graph_file->NewElement("MaterialGraph");
		out_mtl_graph_file->InsertEndChild(mtl_graph_root);
		mtl_graph_root->SetAttribute("NodeCount", m_Delegate.GetNodeCount());
		mtl_graph_root->SetAttribute("LinkCount", m_Delegate.GetLinkCount());

		/* ���нڵ� */
		auto* all_nodes_root = mtl_graph_root->InsertNewChildElement("Nodes");
		for (size_t i = 0; i < m_Delegate.GetNodeCount(); ++i)
		{
			const auto& node = m_Delegate.NodeArray[i];
			auto* node_doc = all_nodes_root->InsertNewChildElement("Node");
			node_doc->SetAttribute("NodeIndex", i);
			node_doc->SetAttribute("NodeType", static_cast<int>(node.NodeType)); // ��֪NodeType������£�Name��TemplateIndex��Width��Height��IsSelected��ʼֵ���ǹ̶��ģ���˲��ر���
			node_doc->SetAttribute("ScreenPosX", node.ScreenPosX);
			node_doc->SetAttribute("ScreenPosY", node.ScreenPosY);

			/* �����ͽڵ���е����� */
			switch (node.NodeType)
			{
			case MaterialGraphNodeType::PBRMaterial:break;
			case MaterialGraphNodeType::Texture2D:
				{
					auto* texture_doc = node_doc->InsertNewChildElement("Texture");
					auto& component = m_Delegate.Registry.get_or_emplace<MGTexture2DComponent>(node.EntityHandle);
					texture_doc->SetAttribute("TexturePath", component.Texture->GetPath().c_str());
					texture_doc->SetAttribute("IsGenMipmap", component.IsGenMipmap);
					texture_doc->SetAttribute("TilingFactor", component.TilingFactor);
				}
				break;
			case MaterialGraphNodeType::SamplerState:
				{
					auto* sampler_state_doc = node_doc->InsertNewChildElement("SamplerState");
					auto& component = m_Delegate.Registry.get_or_emplace<MGSamplerStateComponent>(node.EntityHandle);
					sampler_state_doc->SetAttribute("WrapMode", static_cast<int>(component.WrapMode));
					sampler_state_doc->SetAttribute("MinFilter", static_cast<int>(component.MinFilter));
					sampler_state_doc->SetAttribute("MagFilter", static_cast<int>(component.MagFilter));
				}
				break;
			case MaterialGraphNodeType::Float:
				{
					auto* float_doc = node_doc->InsertNewChildElement("Float");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloatComponent>(node.EntityHandle);
					float_doc->SetAttribute("Value", component.Value);
				}
				break;
			case MaterialGraphNodeType::Float2:
				{
					auto* float2_doc = node_doc->InsertNewChildElement("Float2");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat2Component>(node.EntityHandle);
					float2_doc->SetAttribute("Value", ToString(component.Value).c_str());
				}
				break;
			case MaterialGraphNodeType::Float3:
				{
					auto* float3_doc = node_doc->InsertNewChildElement("Float3");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat3Component>(node.EntityHandle);
					float3_doc->SetAttribute("Value", ToString(component.Value).c_str());
				}
				break;
			case MaterialGraphNodeType::Float4:
				{
					auto* float4_doc = node_doc->InsertNewChildElement("Float4");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat4Component>(node.EntityHandle);
					float4_doc->SetAttribute("Value", ToString(component.Value).c_str());
				}
				break;
			case MaterialGraphNodeType::Color:
				{
					auto* color_doc = node_doc->InsertNewChildElement("Color");
					auto& component = m_Delegate.Registry.get_or_emplace<MGColorComponent>(node.EntityHandle);
					color_doc->SetAttribute("Value", ToString(component.Color).c_str());
				}
				break;
			case MaterialGraphNodeType::Operator:
				{
					auto* operator_doc = node_doc->InsertNewChildElement("Operator");
					auto& component = m_Delegate.Registry.get_or_emplace<MGOperatorComponent>(node.EntityHandle);
					operator_doc->SetAttribute("OperatorType", static_cast<int>(component.OperatorType));
				}
				break;
			}
		}

		/* �������� */
		auto* all_links_root = mtl_graph_root->InsertNewChildElement("Links");
		for (const auto& link : m_Delegate.LinkArray)
		{
			auto* link_doc = all_links_root->InsertNewChildElement("Link");
			link_doc->SetAttribute("InputNodeIndex", link.mInputNodeIndex);
			link_doc->SetAttribute("InputSlotIndex", link.mInputSlotIndex);
			link_doc->SetAttribute("OutputNodeIndex", link.mOutputNodeIndex); // OutputNode��ʾ��ǰ���ߵ�����ˣ�������a->b�����OutputNodeIndexΪb
			link_doc->SetAttribute("OutputSlotIndex", link.mOutputSlotIndex);
		}

		/* ���浽�ı� */
		out_mtl_graph_file->SaveFile(path.c_str());
		delete out_mtl_graph_file;
	}

	/* �����л� */
	void MaterialGraphEditor::Deserializer(const std::string& path)
	{
		PROFILE_FUNCTION();

		m_Path = path;
		ASSERT(!m_Path.empty());

		/* ��ȡ������Ϣ */
		auto* in_mtl_graph_file = new tinyxml2::XMLDocument();
		tinyxml2::XMLError error = in_mtl_graph_file->LoadFile(m_Path.c_str());
		if (error != tinyxml2::XML_SUCCESS)
		{
			CORE_LOG_ERROR("Failed to deserializer mtl file: {}.", m_Path);
			return;
		}

		auto* mtl_graph_root = in_mtl_graph_file->FirstChildElement("MaterialGraph");
		m_Delegate.NodeArray.resize(mtl_graph_root->IntAttribute("NodeCount"));

		/* Nodes */
		auto* all_nodes_root = mtl_graph_root->FirstChildElement("Nodes");
		for (auto* node_doc = all_nodes_root->FirstChildElement("Node"); node_doc; node_doc = node_doc->NextSiblingElement("Node"))
		{
			auto node_index = node_doc->IntAttribute("NodeIndex", -1);
			if (node_index >= 0)
			{
				auto& node = m_Delegate.NodeArray[node_index];
				const auto node_type = static_cast<MaterialGraphNodeType>(node_doc->IntAttribute("NodeType"));
				const auto screen_pos_x = node_doc->FloatAttribute("ScreenPosX");
				const auto screen_pos_y = node_doc->FloatAttribute("ScreenPosY");
				node = m_Delegate.CreateNode(node_type, ImVec2(screen_pos_x, screen_pos_y));

				/* �����ͽڵ���е����� */
				switch (node.NodeType)
				{
				case MaterialGraphNodeType::PBRMaterial:break;
				case MaterialGraphNodeType::Texture2D:
				{
					const auto* texture_doc = node_doc->FirstChildElement("Texture");
					auto& component = m_Delegate.Registry.get_or_emplace<MGTexture2DComponent>(node.EntityHandle);
					component.Texture = TextureAssetManager::Instance().GetOrCreateTexture(texture_doc->Attribute("TexturePath"), {}); // todo: loadConfig
					component.IsGenMipmap = texture_doc->BoolAttribute("IsGenMipmap");
					component.TilingFactor = texture_doc->FloatAttribute("TilingFactor");
				}
				break;
				case MaterialGraphNodeType::SamplerState:
				{
					const auto* sampler_state_doc = node_doc->FirstChildElement("SamplerState");
					auto& component = m_Delegate.Registry.get_or_emplace<MGSamplerStateComponent>(node.EntityHandle);
					component.WrapMode = static_cast<SamplerWrapMode>(sampler_state_doc->IntAttribute("WrapMode"));
					component.MinFilter = static_cast<SamplerMinFilter>(sampler_state_doc->IntAttribute("MinFilter"));
					component.MagFilter = static_cast<SamplerMagFilter>(sampler_state_doc->IntAttribute("MagFilter"));
				}
				break;
				case MaterialGraphNodeType::Float:
				{
					const auto* float_doc = node_doc->FirstChildElement("Float");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloatComponent>(node.EntityHandle);
					component.Value = float_doc->FloatAttribute("Value");
				}
				break;
				case MaterialGraphNodeType::Float2:
				{
					const auto* float2_doc = node_doc->FirstChildElement("Float2");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat2Component>(node.EntityHandle);
					component.Value = ToVec2(float2_doc->Attribute("Value"));
				}
				break;
				case MaterialGraphNodeType::Float3:
				{
					const auto* float3_doc = node_doc->FirstChildElement("Float3");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat3Component>(node.EntityHandle);
					component.Value = ToVec3(float3_doc->Attribute("Value"));
				}
				break;
				case MaterialGraphNodeType::Float4:
				{
					const auto* float4_doc = node_doc->FirstChildElement("Float4");
					auto& component = m_Delegate.Registry.get_or_emplace<MGFloat4Component>(node.EntityHandle);
					component.Value = ToVec4(float4_doc->Attribute("Value"));
				}
				break;
				case MaterialGraphNodeType::Color:
				{
					const auto* color_doc = node_doc->FirstChildElement("Color");
					auto& component = m_Delegate.Registry.get_or_emplace<MGColorComponent>(node.EntityHandle);
					component.Color = ToVec4(color_doc->Attribute("Color"));
				}
				break;
				case MaterialGraphNodeType::Operator:
				{
					const auto* operator_doc = node_doc->FirstChildElement("Operator");
					auto& component = m_Delegate.Registry.get_or_emplace<MGOperatorComponent>(node.EntityHandle);
					component.OperatorType = static_cast<MGNodeOperatorType>(operator_doc->IntAttribute("OperatorType"));
				}
				break;
				}
			}
		}

		/* Links */
		auto* all_links_root = mtl_graph_root->FirstChildElement("Links");
		for (auto* link_doc = all_links_root->FirstChildElement("Link"); link_doc; link_doc = link_doc->NextSiblingElement("Link"))
		{
			m_Delegate.LinkArray.push_back({ 
				static_cast<GraphEditor::NodeIndex>(link_doc->IntAttribute("InputNodeIndex")), 
				static_cast<GraphEditor::NodeIndex>(link_doc->IntAttribute("InputSlotIndex")), 
				static_cast<GraphEditor::NodeIndex>(link_doc->IntAttribute("OutputNodeIndex")), 
				static_cast<GraphEditor::NodeIndex>(link_doc->IntAttribute("OutputSlotIndex"))
			});
		}

		delete in_mtl_graph_file;
		m_IsShow = true;
	}

	/* ������� */
	void MaterialGraphEditor::SaveMaterial()
	{
		// todo
	}

	/* ����ʱ��Ĭ������һ��PBR�ڵ� */
	MaterialGraphEditor::MaterialGraphEditorDelegate::MaterialGraphEditorDelegate()
	{
		NodeArray.emplace_back(CreateNode(MaterialGraphNodeType::PBRMaterial));
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
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), GetEnumName(component.WrapMode));
				draw_list->AddText(ImVec2(rectangle.Min.x, rectangle.Min.y + 20), IM_COL32(255, 128, 64, 255), GetEnumName(component.MinFilter));
				draw_list->AddText(ImVec2(rectangle.Min.x, rectangle.Min.y + 40), IM_COL32(255, 128, 64, 255), GetEnumName(component.MagFilter));
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
				draw_list->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), GetEnumName(component.OperatorType));
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
	MaterialGraphNode MaterialGraphEditor::MaterialGraphEditorDelegate::CreateNode(MaterialGraphNodeType node_type, const ImVec2& ScreenPos)
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
				node_inst.IsSelected = false;
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
		return node_inst;
	}

	/* ������нڵ������ */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::ClearAll()
	{
		NodeArray.clear();
		LinkArray.clear();
	}

	/* ȫѡ�ڵ� */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::SelectAllNodes()
	{
		for (auto& node : NodeArray)
			node.IsSelected = true;
	}

	/* ɾ��ѡ�нڵ� */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::DeleteSelectedNodes()
	{
		/* ��ɾ��ѡ�нڵ��ͬʱ����¼��Щ�ڵ������ */
		std::vector<size_t> selected_node_indices;
		size_t node_index = 0;
		for (auto node_itr = NodeArray.begin(); node_itr != NodeArray.end(); ++node_index)
		{
			if (node_itr->IsSelected)
			{
				node_itr = NodeArray.erase(node_itr);
				selected_node_indices.emplace_back(node_index);
			}
			else
				++node_itr;
		}

		/* ɾ����ѡ�нڵ�ֱ�ӹ��������� */
		for (const auto selected_node_index : selected_node_indices)
		{
			for (auto link_itr = LinkArray.begin(); link_itr != LinkArray.end();)
			{
				/* �����߰���ѡ�нڵ㣬 ��ɾ�������� */
				if (link_itr->mInputNodeIndex == selected_node_index || link_itr->mOutputNodeIndex == selected_node_index)
					link_itr = LinkArray.erase(link_itr);
				else
					++link_itr;
			}
		}

		/* ��¼ʣ��ÿ�����ߵ�������Ҫ���µ�ֵ����Ϊ��ɾ���ڵ�ʱ�����ֽڵ���NodeArray��λ�û���ǰ�ƶ���������Link�������ᵼ�»��Ƴ����Crash */
		std::map<size_t, std::pair<int, int>> update_link_infos; /* <LinkIndex, <InputNodeIndexToSubtract, OutputNodeIndexToSubtract>>*/
		for (const auto selected_node_index : selected_node_indices)
		{
			for (size_t link_index = 0; link_index < LinkArray.size(); ++link_index)
			{
				auto& link = LinkArray[link_index];

				if (link.mInputNodeIndex > selected_node_index)
					update_link_infos[link_index].first++;

				if (link.mOutputNodeIndex > selected_node_index)
					update_link_infos[link_index].second++;
			}
		}

		/* �������ߵ����� */
		for (const auto& link_info : update_link_infos)
		{
			auto& link = LinkArray[link_info.first];
			link.mInputNodeIndex -= link_info.second.first;
			link.mOutputNodeIndex -= link_info.second.second;
		}
	}
}
