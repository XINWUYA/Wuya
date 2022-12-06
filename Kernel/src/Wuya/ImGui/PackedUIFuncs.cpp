#include "Pch.h"
#include "PackedUIFuncs.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Wuya/Application/AssetManager.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya::PackedUIFuncs
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
			const auto& show_texture = texture ? texture : TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/default_texture.png", {});

			/* Image */
			ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					const std::filesystem::path texture_path = path;
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
}
