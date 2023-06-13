#include "Pch.h"
#include "EditorUIFunctions.h"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Wuya
{
	/* 定义注册列表 */
	std::unordered_map<std::string, std::function<void()>> EditorUIFunctions::m_PopupUIRegistry{};

	void EditorUIFunctions::DrawDragFloatUI(const std::string& label, float& value, float label_width)
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

	void EditorUIFunctions::DrawVec3UI(const std::string& label, glm::vec3& values, float reset_value, float label_width)
	{
		PROFILE_FUNCTION();
	}

	void EditorUIFunctions::DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value, float label_width)
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

	void EditorUIFunctions::DrawColorUI(const std::string& label, glm::vec4& color, float label_width)
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

	void EditorUIFunctions::DrawTextureUI(const std::string& label, SharedPtr<Texture>& texture, float& tiling_factor, float label_width)
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
			const auto& show_texture = texture ? texture : TextureAssetManager::Instance().GetOrCreateTexture(RELATIVE_PATH("EditorRes/icons/Default.png"));

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

	void EditorUIFunctions::DrawCheckedImageButtonUI(const std::string& label, const SharedPtr<Texture>& texture, const ImVec2& size, bool checked, const std::function<void()>& button_func)
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

	void EditorUIFunctions::DrawCheckboxUI(const std::string& label, bool& value, float label_width)
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

	void EditorUIFunctions::DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback, float label_width)
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

	bool EditorUIFunctions::DrawModalUI(const std::string& label, const std::string& content_text, bool& never_ask)
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

	void EditorUIFunctions::RegisterPopupFunc(const std::string& label, std::function<void()> func)
	{
		m_PopupUIRegistry[label] = std::move(func);
	}

	void EditorUIFunctions::DrawPopups()
	{
		for (auto& popup_ui : m_PopupUIRegistry)
		{
			popup_ui.second();
		}
	}

	std::string ConvertFloat2String(float value, int precision)
	{
		if (precision < 0)
			return std::to_string(value);

		std::ostringstream oss;
		oss.precision(precision);
		oss << std::fixed << value;
		return oss.str();
	}

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		PROFILE_FUNCTION();

		/* Ref: glm::decompose() */

		glm::mat4 transform_t = transform;
		/* 判断矩阵有效 */
		if (glm::epsilonEqual(transform_t[3][3], 0.0f, glm::epsilon<float>()))
			return false;

		/* 先处理变换矩阵最后一行，不考虑仿射变换 */
		if (glm::epsilonNotEqual(transform_t[0][3], 0.0f, glm::epsilon<float>()) ||
			glm::epsilonNotEqual(transform_t[1][3], 0.0f, glm::epsilon<float>()) ||
			glm::epsilonNotEqual(transform_t[2][3], 0.0f, glm::epsilon<float>()))
		{
			/* 清除Perspective部分 */
			transform_t[0][3] = 0.0f;
			transform_t[1][3] = 0.0f;
			transform_t[2][3] = 0.0f;
			transform_t[3][3] = 1.0f;
		}

		/* 获取translation */
		translation = glm::vec3(transform_t[3]);

		/* 清空变换矩阵translation信息 */
		transform_t[3] = glm::vec4(0.0f, 0.0f, 0.0f, transform_t[3][3]);

		glm::vec3 row[3];
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				row[i][j] = transform_t[i][j];

		/* 获取scale */
		scale.x = glm::length(row[0]);
		row[0] = glm::detail::scale(row[0], 1.0f);
		scale.y = glm::length(row[1]);
		row[1] = glm::detail::scale(row[1], 1.0f);
		scale.z = glm::length(row[2]);
		row[2] = glm::detail::scale(row[2], 1.0f);

		/* 获取rotation */
		rotation.y = asin(-row[0][2]);
		if (cos(rotation.y) != 0.0f) {
			rotation.x = atan2(row[1][2], row[2][2]);
			rotation.z = atan2(row[0][1], row[0][0]);
		}
		else {
			rotation.x = atan2(-row[2][0], row[1][1]);
			rotation.z = 0.0f;
		}

#if 0
		/* 列优先改为行优先 */
		glm::mat3 mat = transpose(glm::mat3(transform_t));

		/* 获取scale */
		scale = glm::vec3(glm::length(mat[0]), glm::length(mat[1]), glm::length(mat[2]));

		mat[0] = glm::detail::scale(mat[0], 1.0f);
		mat[1] = glm::detail::scale(mat[1], 1.0f);
		mat[2] = glm::detail::scale(mat[2], 1.0f);

		/* 获取rotation */
		rotation.y = asin(-mat[0][2]);
		if (cos(rotation.y) != 0.0f)
		{
			rotation.x = atan2(mat[1][2], mat[2][2]);
			rotation.z = atan2(mat[0][1], mat[0][0]);
		}
		else
		{
			rotation.x = atan2(-mat[2][0], mat[1][1]);
			rotation.z = 0;
		}
#endif

		return true;
	}
}