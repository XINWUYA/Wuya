#include "pch.h"
#include "EditorUIFunctions.h"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include "EditorAssetManager.h"
#include "Wuya/Core/Logger.h"

extern const std::filesystem::path g_AssetPath;

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

void EditorUIFunctions::DrawTextureUI(const std::string& label, Wuya::SharedPtr<Wuya::Texture2D>& texture, float& tiling_factor, float label_width)
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
		const auto show_texture = texture ? texture : EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/default_texture.png");

		/* Image */
		ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				const std::filesystem::path texture_path = g_AssetPath / path;
				const auto new_texture = EditorAssetManager::Instance()->GetOrCreateTexture(texture_path.string());
				if (new_texture->IsLoaded())
					texture = new_texture;
				else
					EDITOR_LOG_WARN("Failed to load texture {0}.", texture_path.filename().string());
			}
			ImGui::EndDragDropTarget();
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
