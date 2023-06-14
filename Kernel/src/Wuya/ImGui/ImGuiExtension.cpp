#include "Pch.h"

#include <filesystem>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "ImGuiExtensions.h"
#include "Wuya/Application/AssetManager.h"
#include "Wuya/Renderer/Texture.h"
#include "Wuya/Scene/SceneCommon.h"

namespace Wuya::ImGuiExt
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
			const auto& show_texture = texture ? texture : TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Default.png"));

			/* Image */
			ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					const auto new_texture = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(path));
					if (new_texture->IsLoaded())
						texture = new_texture;
					else
					{
						const std::filesystem::path texture_path = g_AssetsPath / path;
						EDITOR_LOG_WARN("Failed to load texture {0}.", texture_path.filename().string());
					}
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
			ImGui::TextWrapped(RELATIVE_PATH(show_texture->GetPath()).c_str());

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

	void DrawDragFloatUI(const std::string& label, float& value, float label_width)
	{
		PROFILE_FUNCTION();

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);

		/* Label */
		ImGui::SetColumnWidth(0, label_width);
		ImGui::Text(label.c_str());

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

	void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value, float label_width)
	{
		PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		const auto bold_font = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		/* Label */
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, label_width);
		ImGui::Text(label.c_str());
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
			values.x = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		/* Y */
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(bold_font);
		if (ImGui::Button("Y", button_size))
			values.y = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		/* Z */
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(bold_font);
		if (ImGui::Button("Z", button_size))
			values.z = reset_value;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void DrawCheckedImageButtonUI(const std::string& label, const SharedPtr<Texture>& texture, const ImVec2& size, bool checked, const std::function<void()>& button_func)
	{
		PROFILE_FUNCTION();

		ASSERT(texture);

		/* 选中时样式 */
		if (checked)
		{
			const auto& colors = ImGui::GetStyle().Colors;
			const auto& button_active_color = colors[ImGuiCol_ButtonActive];
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(button_active_color.x, button_active_color.y, button_active_color.z, 0.5f));
		}

		/* Button */
		if (ImGui::ImageButton((ImTextureID)texture->GetTextureID(), size, ImVec2(0, 1), ImVec2(1, 0), 0))
		{
			button_func();
		}

		/* 鼠标悬停提示 */
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(label.c_str());

		if (checked)
		{
			ImGui::PopStyleColor();
		}
	}

	void DrawCheckboxUI(const std::string& label, bool& value, float label_width)
	{
		PROFILE_FUNCTION();

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);

		/* Label */
		ImGui::SetColumnWidth(0, label_width);
		ImGui::Text(label.c_str());

		/* Checkbox */
		ImGui::NextColumn();
		ImGui::Checkbox("##Checkbox", &value);

		ImGui::Columns(1);
		ImGui::PopID();
	}

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

	bool DrawModalUI(const std::string& label, const std::string& content_text, bool& never_ask)
	{
		PROFILE_FUNCTION();

		/* 显示在窗口中间 */
		const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		bool result = false;
		if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::TextWrapped(content_text.c_str());
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::Checkbox("Don't ask me next time", &never_ask);
			ImGui::PopStyleVar();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				result = true;
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				result = false;
			}
			ImGui::EndPopup();
		}

		return result;
	}

}
