#include "Pch.h"
#include "PackedUIFuncs.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Wuya/Application/AssetManager.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	namespace PackedUIFuncs
	{
		/* 绘制一个普通的文本UI */
		void DrawCommonTextUI(const std::string& label, const std::string& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label.c_str());

			/* Text */
			ImGui::NextColumn();
			ImGui::TextWrapped(value.c_str());

			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个Color UI */
		void DrawColorUI(const std::string& label, glm::vec4& color, float label_width)
		{
			PROFILE_FUNCTION();

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label.c_str());

			/* Color */
			ImGui::NextColumn();
			ImGui::ColorEdit4("##Color", glm::value_ptr(color));

			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个图片UI */
		void DrawTextureUI(const std::string& label, SharedPtr<Texture>& texture, float& tiling_factor, float label_width)
		{
			PROFILE_FUNCTION();

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label.c_str());

			/* Texture */
			ImGui::NextColumn();
			{
				const auto& show_texture = texture ? texture : TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/Default.png", {});

				/* Image */
				ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						const std::filesystem::path texture_path = g_AssetsPath / path;
						const auto new_texture = TextureAssetManager::Instance().GetOrCreateTexture(texture_path.string(), {});
						if (new_texture->IsLoaded())
							texture = new_texture;
						else
							EDITOR_LOG_WARN("Failed to load texture {0}.", texture_path.filename().string());
					}
					ImGui::EndDragDropTarget();
				}

				/* Hovered */
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image((ImTextureID)show_texture->GetTextureID(), ImVec2(240, 240), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::EndTooltip();
				}

				/* Path */
				ImGui::SameLine();
				ImGui::TextWrapped(show_texture->GetPath().c_str());

				/* Tiling Factor */
				ImGui::DragFloat("Tiling Factor", &tiling_factor, 0.1f, 0.0f, 100.0f);
			}

			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个可拖动的Int UI */
		void DrawDragIntUI(const char* label, int& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGui::PushID(label);
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label);

			/* DragFloat */
			ImGui::NextColumn();
			ImGui::DragInt("##Int", &value);

			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个可拖动的Float UI */
		void DrawDragFloatUI(const char* label, float& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGui::PushID(label);
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label);

			/* DragFloat */
			ImGui::NextColumn();
			ImGui::DragFloat("##Float", &value);

			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个可拖动的Float2 UI */
		void DrawDragFloat2UI(const char* label, glm::vec2& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGuiIO& io = ImGui::GetIO();
			const auto bold_font = io.Fonts->Fonts[0];

			ImGui::PushID(label);
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label);
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			const float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };

			/* X */
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("X", button_size))
				value.x = 0;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* Y */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("Y", button_size))
				value.y = 0;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个可拖动的Float3 UI */
		void DrawDragFloat3UI(const char* label, glm::vec3& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGuiIO& io = ImGui::GetIO();
			const auto bold_font = io.Fonts->Fonts[0];

			ImGui::PushID(label);

			/* Label */
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label);
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			const float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };

			/* X */
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("X", button_size))
				value.x = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* Y */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("Y", button_size))
				value.y = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* Z */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("Z", button_size))
				value.z = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个可拖动的Float4 UI */
		void DrawDragFloat4UI(const char* label, glm::vec4& value, float label_width)
		{
			PROFILE_FUNCTION();

			ImGuiIO& io = ImGui::GetIO();
			const auto bold_font = io.Fonts->Fonts[0];

			ImGui::PushID(label);

			/* Label */
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label);
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			const float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			const ImVec2 button_size = { line_height + 3.0f, line_height };

			/* X */
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("X", button_size))
				value.x = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* Y */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("Y", button_size))
				value.y = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* Z */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("Z", button_size))
				value.z = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			/* W */
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(bold_font);
			if (ImGui::Button("W", button_size))
				value.z = 0.0f;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##W", &value.w, 0.1f, 0.0f, 0.0f, "%.3f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();
			ImGui::Columns(1);
			ImGui::PopID();
		}

		/* 绘制一个Combo */
		void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback, float label_width)
		{
			PROFILE_FUNCTION();

			ASSERT(!options.empty());

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);

			/* Label */
			ImGui::SetColumnWidth(0, label_width);
			ImGui::Text(label.c_str());

			/* Combo */
			ImGui::NextColumn();
			{
				std::vector<const char*> option_strs;
				for (const auto& option : options)
					option_strs.emplace_back(option.c_str());
				const char* selected_option = option_strs[selected_idx];
				if (ImGui::BeginCombo("##Combo", selected_option))
				{
					for (int i = 0; i < (int)options.size(); i++)
					{
						bool is_selectd = selected_option == option_strs[i];
						if (ImGui::Selectable(option_strs[i], is_selectd))
						{
							selected_option = option_strs[i];
							selected_idx = i;
							callback(i);
						}

						if (is_selectd)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			ImGui::Columns(1);
			ImGui::PopID();
		}
	}
}
