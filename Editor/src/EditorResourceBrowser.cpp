#include "EditorResourceBrowser.h"
#include <imgui.h>

#include "imgui_internal.h"

extern const std::filesystem::path g_AssetPath = "assets";
EditorResourceBrowser::EditorResourceBrowser()
	: m_CurrentDirectory(g_AssetPath)
{
	m_pDirectoryIcon = Wuya::Texture2D::Create("editor_res/icons/directory_icon.png");
	m_pFileIcon = Wuya::Texture2D::Create("editor_res/icons/file_icon.png");
	m_pFilterIcon = Wuya::Texture2D::Create("editor_res/icons/filter_icon.png");
}

void EditorResourceBrowser::OnImGuiRenderer()
{
	ImGui::Begin("Resource Browser");
	{
		// Back
		{
			bool disable_return_btn = true;
			if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
				disable_return_btn = false;

			if (disable_return_btn)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button(" << "))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			if (disable_return_btn)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}

		static float padding = 16.0f;
		static float thumbnail_size = 64.0f;

		// Slider Controller
		{
			ImGui::PushItemWidth(100);

			ImGui::SameLine(80);
			ImGui::Text("Size: ");
			ImGui::SameLine();
			ImGui::SliderFloat("##", &thumbnail_size, 16, 128);
			ImGui::SameLine(240);
			ImGui::Text("Padding: ");
			ImGui::SameLine();
			ImGui::SliderFloat("##", &padding, 0, 32);

			ImGui::PopItemWidth();
		}

		const float panel_width = ImGui::GetContentRegionAvail().x;

		// Filter
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		static ImGuiTextFilter filter;
		ImGui::SameLine(panel_width - 160);
		filter.Draw();
		ImGui::SameLine(panel_width - 8);
		const Wuya::SharedPtr<Wuya::Texture2D> filter_icon = m_pFilterIcon;
		ImGui::Image((ImTextureID)filter_icon->GetTextureID(), ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::PopStyleColor();

		ImGui::Separator();

		// Files
		const float cell_size = thumbnail_size + padding;
		int column_count = static_cast<int>(panel_width / cell_size);
		if (column_count < 1)
			column_count = 1;

		ImGui::Columns(column_count, 0, false);
		for (auto& directory_entry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directory_entry.path();
			auto relative_path = std::filesystem::relative(path, g_AssetPath);
			std::string filename = relative_path.filename().string();

			if (filter.PassFilter(filename.c_str()))
			{
				ImGui::PushID(filename.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				// 显示图标
				const Wuya::SharedPtr<Wuya::Texture2D> icon_texture = directory_entry.is_directory() ? m_pDirectoryIcon : m_pFileIcon;
				ImGui::ImageButton((ImTextureID)icon_texture->GetTextureID(), ImVec2(thumbnail_size, thumbnail_size), ImVec2(0, 1), ImVec2(1, 0));

				// 拖动
				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* item_path = relative_path.c_str();
					ImGui::SetDragDropPayload("RESOURCE_BROWSER_ITEM", item_path, (wcslen(item_path) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();

				// 双击打开
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directory_entry.is_directory())
						m_CurrentDirectory /= path.filename();
				}
				ImGui::TextWrapped(filename.c_str());

				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
		ImGui::Columns(1);
	}
	ImGui::End();
}
