#include "Pch.h"
#include "MaterialGraphEditor.h"

#include "PackedUIFuncs.h"

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	MaterialGraphEditor::MaterialGraphEditor()
	{
		/* 默认显示所有端口名 */
		m_Options.mDrawIONameOnHover = false;
		m_Options.mLineThickness = 2.0f; /* 连线粗细 */
		m_Options.mNodeSlotRadius = 6.0f; /* 连接点的半径 */
	}

	/* 显示/隐藏编辑器 */
	void MaterialGraphEditor::ShowOrHide()
	{
		m_IsShow = !m_IsShow;
	}

	/* 绘制相关UI */
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
			/* 保存场景按钮 */
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

			/* 显示节点图 */
			GraphEditor::Show(m_Delegate, m_Options, m_ViewState, true, &m_FitMode);

			/* 空白处右键，唤出新建 */
			if (m_Delegate.m_IsRightClickEmpty)
				ImGui::OpenPopup("CreateNodePopup");

			/* 显示 */
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

			/* 展开弹窗时，显示控件 */
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

		/* 显示选中节点的属性，便于编辑 */
		ImGui::Begin("Properties");
		{
			PROFILE_SCOPE("Show Material Graph Node's Properties")

			/* 即使多选，也只显示第一个节点的属性 */
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

			/* 存在选中节点 */
			if (selected_node_idx > 0)
			{
				auto& node = m_Delegate.NodeArray[selected_node_idx];
				switch (node.NodeType)
				{
				case MaterialGraphNodeType::PBRMaterial: break;
				case MaterialGraphNodeType::Texture2D:
					{
						/* 显示组件 */
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

						/* 暂不支持多元操作符，麻烦太多 */
						//int element_cnt_index = component.ElementCnt - 2;
						//PackedUIFuncs::DrawComboUI("ElementCnt", {"2", "3", "4"}, element_cnt_index);
						///* 当元素数量发生改变时 */
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

	/* 保存材质 */
	void MaterialGraphEditor::SaveMaterial()
	{
		// todo
	}

	/* 构造时，默认增加一个PBR节点 */
	MaterialGraphEditor::MaterialGraphEditorDelegate::MaterialGraphEditorDelegate()
	{
		CreateNode(MaterialGraphNodeType::PBRMaterial);
	}

	/* 支持连线 */
	bool MaterialGraphEditor::MaterialGraphEditorDelegate::AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to)
	{
		return true;
	}

	/* 选中节点 */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::SelectNode(GraphEditor::NodeIndex node_index, bool selected)
	{
		NodeArray[node_index].IsSelected = selected;
	}

	/* 移动选中节点 */
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

	/* 连线 */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::AddLink(GraphEditor::NodeIndex input_node_index, GraphEditor::SlotIndex input_slot_index, GraphEditor::NodeIndex output_node_index, GraphEditor::SlotIndex output_slot_index)
	{
		LinkArray.push_back({ input_node_index, input_slot_index, output_node_index, output_slot_index });
	}

	/* 删除连线 */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::DelLink(GraphEditor::LinkIndex link_index)
	{
		LinkArray.erase(LinkArray.begin() + link_index);
	}

	/* 自定义绘制 */
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

				/* 鼠标掠过时显示图片路径 */
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

	/* 右键响应，只在右键到节点的端口时才生效 */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::RightClick(GraphEditor::NodeIndex node_index, GraphEditor::SlotIndex slot_index_input, GraphEditor::SlotIndex slot_index_output)
	{
		if (node_index != -1)
		{
			auto& node = NodeArray[node_index];

			return;
		}

		m_IsRightClickEmpty = true;
	}

	/* 获取节点模板数量 */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplateCount()
	{
		return s_Templates.size();
	}

	/* 获取指定节点模板 */
	const GraphEditor::Template MaterialGraphEditor::MaterialGraphEditorDelegate::GetTemplate(GraphEditor::TemplateIndex index)
	{
		ASSERT(index < s_Templates.size());

		return s_Templates[index];
	}

	/* 获取节点数量 */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetNodeCount()
	{
		return NodeArray.size();
	}

	/* 获取指定节点 */
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

	/* 获取连线数量 */
	const size_t MaterialGraphEditor::MaterialGraphEditorDelegate::GetLinkCount()
	{
		return LinkArray.size();
	}

	/* 获取指定连线 */
	const GraphEditor::Link MaterialGraphEditor::MaterialGraphEditorDelegate::GetLink(GraphEditor::LinkIndex index)
	{
		ASSERT(index < LinkArray.size());

		return LinkArray[index];
	}

	/* 创建一个指定类型的Node */
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
				node_inst.IsSelected = true; // 默认被选中，用于首次显示时适应到屏幕
			}
			break;
		case MaterialGraphNodeType::Texture2D:
			{
				node_inst.Name = "Texture2D";
				node_inst.TemplateIndex = TemplateIndex_Texture2D;
				node_inst.Width = 160;
				node_inst.Height = 180;
				node_inst.IsSelected = false;

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
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

				/* 挂载相关组件 */
				node_inst.EntityHandle = Registry.create();
				auto& component = Registry.emplace<MGOperatorComponent>(node_inst.EntityHandle);
			}
			break;
		}

		NodeArray.emplace_back(node_inst);
	}

	/* 清楚所有节点和连线 */
	void MaterialGraphEditor::MaterialGraphEditorDelegate::ClearAll()
	{
		NodeArray.clear();
		LinkArray.clear();
	}
}
