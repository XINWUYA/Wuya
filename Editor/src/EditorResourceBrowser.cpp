#include "Pch.h"
#include "EditorResourceBrowser.h"
#include "EditorAssetManager.h"
#include "EditorUIFunctions.h"

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath = "assets";
	EditorResourceBrowser::EditorResourceBrowser()
	{
		PROFILE_FUNCTION();

		m_pFolderIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/directory.png");
		m_pFileIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/file.png");
		m_pFilterIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/filter.png");
		m_pMenuIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/menu.png");
		m_pReturnIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/return.png");
	}

	void EditorResourceBrowser::OnImGuiRenderer()
	{
		PROFILE_FUNCTION();

		/* ���������ļ�Ŀ¼�ڵ��� */
		if (m_IsDirty)
		{
			m_RootFileNodeTree = CreateSharedPtr<FileNode>(g_AssetPath.string(), "", "Folder", 0, -1);
			BuildFileNodeTree(m_RootFileNodeTree);
		}

		/* �ļ�Ŀ¼�б��� */
		ImGui::Begin("File List");
		{
			if (ImGui::CollapsingHeader(g_AssetPath.string().c_str()))
			{
				/* �����ļ��ڵ�������UI */
				BuildFileUIListTreeSimple(m_RootFileNodeTree);
			}
		}
		ImGui::End();

		/* ��ϸ��Դ�б��� */
		ImGui::Begin("Resource Browser");
		{
			if (!m_CurrentFileNode)
				m_CurrentFileNode = m_RootFileNodeTree; /* Ĭ��Ϊ���ڵ� */

			/* ��һ��Ŀ¼ͼ�� */
			{
				const bool disable_return_btn = (*m_CurrentFileNode) == (*m_RootFileNodeTree);

				/* Button��ʽ */
				float button_alpha = 0.5f;
				if (disable_return_btn)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					button_alpha = 0.25f;
				}

				START_STYLE_ALPHA(button_alpha);
				if (ImGui::ImageButton((ImTextureID)m_pReturnIcon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					m_CurrentFileNode = m_CurrentFileNode->ParentNode;
				END_STYLE_ALPHA;

				/* ȡ����ǰ��ʽ */
				if (disable_return_btn)
					ImGui::PopItemFlag();
			}

			/* ���������� */
			const float panel_width = ImGui::GetContentRegionAvail().x;

			/* ͼ���С�ͼ�� */
			static float thumbnail_size = 64.0f;
			static float padding = 32.0f;

			/* ͸��Button��ʽ */
			START_TRANSPARENT_BUTTON;

			/* ɸѡ���� */
			static ImGuiTextFilter filter;
			{
				ImGui::SameLine(40, 20);
				filter.Draw("##", 200);
				ImGui::SameLine(236);
				const SharedPtr<Texture2D> filter_icon = m_pFilterIcon;

				START_STYLE_ALPHA(0.5f);
				ImGui::Image((ImTextureID)filter_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
				END_STYLE_ALPHA;
			}

			/* ͼ���С�ͼ�����ڿؼ� */
			{
				ImGui::SameLine(panel_width - 15);
				START_STYLE_ALPHA(0.5f);
				if (ImGui::ImageButton((ImTextureID)m_pMenuIcon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					ImGui::OpenPopup("SettingPopup");
				END_STYLE_ALPHA;

				/* չ������ʱ����ʾ�ؼ� */
				if (ImGui::BeginPopup("SettingPopup"))
				{
					ImGui::PushItemWidth(120);

					ImGui::SliderFloat("Size", &thumbnail_size, 16, 128);
					ImGui::Separator();
					ImGui::SliderFloat("Padding", &padding, 0, 32);

					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
			}

			/* ȡ����ǰ��ʽ */
			END_TRANSPARENT_BUTTON;

			/* �ָ��� */
			ImGui::Separator();

			/* �ļ�UI, �㹻��ʱ����ʾͼƬ���������嵥��ʽ��ʾ */
			if (thumbnail_size > 32)
			{
				const float cell_size = thumbnail_size + padding;
				int column_count = static_cast<int>(panel_width / cell_size);
				if (column_count < 1)
					column_count = 1;

				ImGui::Columns(column_count, 0, false);

				for (const auto& child_node : m_CurrentFileNode->ChildNodes)
				{
					auto filepath = std::filesystem::path(child_node->FilePath);

					/* ͨ��ɸѡ�Ĳ���Ҫ��ʾ */
					if (filter.PassFilter(child_node->FilePath.c_str()))
					{
						ImGui::PushID(child_node->FileName.c_str());
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
							{
								/* ͼ�� */
								const SharedPtr<Texture2D> icon_texture = child_node->FileType == "Folder" ? m_pFolderIcon : m_pFileIcon;
								ImGui::ImageButton((ImTextureID)icon_texture->GetTextureID(), ImVec2(thumbnail_size, thumbnail_size), ImVec2(0, 1), ImVec2(1, 0));

								/* �϶� */
								if (ImGui::BeginDragDropSource())
								{
									const wchar_t* item_path = filepath.c_str();
									ImGui::SetDragDropPayload("RESOURCE_BROWSER_ITEM", item_path, (wcslen(item_path) + 1) * sizeof(wchar_t)); /* todo: */
									ImGui::EndDragDropSource();
								}
							}
							ImGui::PopStyleColor();

							/* ˫���� */
							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (child_node->FileType == "Folder")
									m_CurrentFileNode = child_node;
							}

							/* �Ҽ�ѡ����� */
							if (ImGui::BeginPopupContextItem())
							{
								/* ��ϵͳ�ļ�������ʾ */
								if (ImGui::MenuItem("Show in file explorer"))
								{
									/* todo: ������ϵͳ�ļ��� */
									std::string path = g_AssetPath.string() + "\\" + child_node->FilePath;
									EDITOR_LOG_DEBUG("File Abs Path: {}.", path);
								}

								ImGui::EndPopup();
							}

							/* �ļ��� */
							ImGui::TextWrapped(child_node->FileName.c_str());

							/* ��һ���ļ� */
							ImGui::NextColumn();
						}
						ImGui::PopID();
					}
				}
				ImGui::Columns(1);
			}
			else /* ��ͼ���Сʱ��ת��Ϊ�б�ģʽ */
			{
				ImGui::BeginTable("Assets List", 3);
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					/* �����ļ��ڵ�������UI */
					BuildFileUIListTreeDetail(m_CurrentFileNode);
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	void EditorResourceBrowser::BuildFileNodeTree(const SharedPtr<FileNode>& parent_node)
	{
		for (auto& directory_entry : std::filesystem::directory_iterator(g_AssetPath / parent_node->FilePath))
		{
			const auto& path = directory_entry.path();
			auto relative_path = GetRelativePath(g_AssetPath, path);
			auto filename = relative_path.filename();

			/* ����һ���ļ��ڵ� */
			auto file_node = CreateSharedPtr<FileNode>();
			file_node->ParentNode = parent_node;
			file_node->FileName = filename.string();
			file_node->FilePath = relative_path.string();
			file_node->Depth = parent_node->Depth + 1;

			if (directory_entry.is_directory()) /* �ļ��У��ݹ���Ŀ¼ */
			{
				file_node->FileType = "Folder";
				parent_node->ChildNodes.emplace_back(file_node);
				BuildFileNodeTree(file_node);
			}
			else
			{
				auto ext = filename.extension().string();
				transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
				file_node->FileType = ext;
				file_node->FileSize = static_cast<float>(std::filesystem::file_size(path)) / 1024.0f;
				parent_node->ChildNodes.emplace_back(file_node);
			}
		}
		m_IsDirty = false;
	}

	void EditorResourceBrowser::BuildFileUIListTreeDetail(const SharedPtr<FileNode>& node)
	{
		for (const auto& child_node : node->ChildNodes)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			if (child_node->FileType == "Folder") /* �ļ��� */
			{
				/* �ļ��ڵ� */
				bool open = ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth);

				/* �ļ����� */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::TextUnformatted(child_node->FileType.c_str());

				/* �ļ���С */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(300);
				ImGui::TextUnformatted("");

				/* չ���ļ��нڵ� */
				if (open)
				{
					BuildFileUIListTreeDetail(child_node);
					ImGui::TreePop();
				}
			}
			else
			{
				/* �ļ��ڵ� */
				ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

				/* todo: �����ļ�ʱ����Ӧ */
				if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen())
				{
					/**/
				}

				/* �Ҽ�ѡ����� */
				if (ImGui::BeginPopupContextItem())
				{
					/* ��ϵͳ�ļ�������ʾ */
					if (ImGui::MenuItem("Show in file explorer"))
					{
						/* todo: ������ϵͳ�ļ��� */
						std::string path = g_AssetPath.string() + "\\" + child_node->FilePath;
						EDITOR_LOG_DEBUG("File Abs Path: {}.", path);
					}

					ImGui::EndPopup();
				}

				/* �ļ����� */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(160);
				ImGui::TextUnformatted(child_node->FileType.c_str());

				/* �ļ���С */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(300);
				ImGui::TextUnformatted((ConvertFloat2String(child_node->FileSize, 2) + "KB").c_str());
			}
		}
	}

	void EditorResourceBrowser::BuildFileUIListTreeSimple(const SharedPtr<FileNode>& node)
	{
		for (const auto& child_node : node->ChildNodes)
		{
			if (child_node->FileType == "Folder") /* �ļ��� */
			{
				/* �ļ��ڵ� */
				bool open = ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth);

				/* ���Ŀ¼ʱ������Ϊ��ǰ�ļ�Ŀ¼ */
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					m_CurrentFileNode = child_node;

				/* չ���ļ��нڵ� */
				if (open)
				{
					BuildFileUIListTreeSimple(child_node);
					ImGui::TreePop();
				}
			}
			else
			{
				/* �ļ��ڵ� */
				ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

				/* todo: �����ļ�ʱ����Ӧ */
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				{
					/**/
				}
			}
		}
	}

	std::filesystem::path EditorResourceBrowser::GetRelativePath(const std::filesystem::path& dir, const std::filesystem::path& path)
	{
		return path.lexically_relative(dir);
	}
}