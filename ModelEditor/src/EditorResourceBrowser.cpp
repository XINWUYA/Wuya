#include "Pch.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	void EditorResourceBrowser::OnImGuiRenderer()
	{
		PROFILE_FUNCTION();

		/* 重新生成文件目录节点树 */
		if (m_IsDirty)
		{
			m_RootFileNodeTree = CreateSharedPtr<FileNode>(g_AssetsPath.string(), "", FileType::Folder, 0, -1);
			BuildFileNodeTree(m_RootFileNodeTree);
		}

		/* 文件目录列表窗口 */
		ImGui::Begin("File List");
		{
			if (ImGui::CollapsingHeader(g_AssetsPath.string().c_str()))
			{
				/* 根据文件节点树生成UI */
				BuildFileUIListTreeSimple(m_RootFileNodeTree);
			}
		}
		ImGui::End();

		/* 详细资源列表窗口 */
		ImGui::Begin("Resource Browser");
		{
			if (!m_CurrentFileNode)
				m_CurrentFileNode = m_RootFileNodeTree; /* 默认为根节点 */

			/* 上一级目录图标 */
			{
				const bool disable_return_btn = (*m_CurrentFileNode) == (*m_RootFileNodeTree);

				/* Button样式 */
				float button_alpha = 0.5f;
				if (disable_return_btn)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					button_alpha = 0.25f;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * button_alpha);
				const auto return_icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/return.png"));
				if (ImGui::ImageButton((ImTextureID)return_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					m_CurrentFileNode = m_CurrentFileNode->ParentNode.lock();
				ImGui::PopStyleVar();

				/* 取消当前样式 */
				if (disable_return_btn)
					ImGui::PopItemFlag();
			}

			/* 窗口区域宽度 */
			const float panel_width = ImGui::GetContentRegionAvail().x;

			/* 图标大小和间隔 */
			static float thumbnail_size = 64.0f;
			static float padding = 32.0f;

			/* 透明Button样式 */
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			/* 筛选窗口 */
			static ImGuiTextFilter filter;
			{
				ImGui::SameLine(40, 20);
				filter.Draw("##", 200);
				ImGui::SameLine(236);
				const auto filter_icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/filter.png"));

				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Image((ImTextureID)filter_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0));
				ImGui::PopStyleVar();
			}

			/* 图标大小和间隔调节控件 */
			{
				ImGui::SameLine(panel_width - 15);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				const auto menu_icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/menu.png"));
				if (ImGui::ImageButton((ImTextureID)menu_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					ImGui::OpenPopup("SettingPopup");
				ImGui::PopStyleVar();

				/* 展开弹窗时，显示控件 */
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

			/* 取消透明样式 */
			ImGui::PopStyleColor();

			/* 分割线 */
			ImGui::Separator();

			/* 文件UI, 足够大时才显示图片，否则以清单形式显示 */
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

					/* 通过筛选的才需要显示 */
					if (filter.PassFilter(child_node->FilePath.c_str()))
					{
						ImGui::PushID(child_node->FileName.c_str());
						{
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
							{
								/* 图标 */
								ImGui::ImageButton((ImTextureID)child_node->Icon->GetTextureID(), ImVec2(thumbnail_size, thumbnail_size), ImVec2(0, 1), ImVec2(1, 0));

								/* 拖动 */
								if (ImGui::BeginDragDropSource())
								{
									const wchar_t* item_path = filepath.c_str();
									ImGui::SetDragDropPayload("RESOURCE_BROWSER_ITEM", item_path, (wcslen(item_path) + 1) * sizeof(wchar_t)); /* todo: */
									ImGui::EndDragDropSource();
								}
							}
							ImGui::PopStyleColor();

							/* 双击打开 */
							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (child_node->FileType == FileType::Folder)
									m_CurrentFileNode = child_node;
							}

							/* 右键选项操作 */
							if (ImGui::BeginPopupContextItem())
							{
								/* 在系统文件夹中显示 */
								if (ImGui::MenuItem("Show in file explorer"))
								{
									auto path = g_AssetsPath / child_node->FilePath;
									path = absolute(path).make_preferred();
									OpenFileExplorer(path.string().c_str());
								}

								ImGui::EndPopup();
							}

							/* 文件名 */
							ImGui::TextWrapped(child_node->FileName.c_str());

							/* 下一个文件 */
							ImGui::NextColumn();
						}
						ImGui::PopID();
					}
				}
				ImGui::Columns(1);
			}
			else /* 当图标过小时，转换为列表模式 */
			{
				ImGui::BeginTable("Assets List", 3);
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					/* 根据文件节点树生成UI */
					BuildFileUIListTreeDetail(m_CurrentFileNode);
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	void EditorResourceBrowser::BuildFileNodeTree(const SharedPtr<FileNode>& parent_node)
	{
		for (auto& directory_entry : std::filesystem::directory_iterator(g_AssetsPath / parent_node->FilePath))
		{
			const auto& path = directory_entry.path();
			auto relative_path = GetRelativePath(g_AssetsPath, path);
			auto filename = relative_path.filename();

			/* 构造一个文件节点 */
			auto file_node = CreateSharedPtr<FileNode>();
			file_node->ParentNode = parent_node;
			file_node->FileName = filename.string();
			file_node->FilePath = relative_path.string();
			file_node->Depth = parent_node->Depth + 1;

			if (directory_entry.is_directory()) /* 文件夹，递归子目录 */
			{
				file_node->FileType = FileType::Folder;
				file_node->Icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/directory.png"));
				parent_node->ChildNodes.emplace_back(file_node);
				BuildFileNodeTree(file_node);
			}
			else
			{
				auto ext = filename.extension().string();
				transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
				if (ext == ".JPG" || ext == ".PNG" || ext == ".DDS" || ext == ".TGA" || ext == ".BMP")
				{
					file_node->FileType = FileType::Image;
					file_node->Icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(file_node->FilePath));
				}
				else if (ext == ".SCN")
				{
					file_node->FileType = FileType::Scene;
					file_node->Icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/file_scn.png"));

				}
				else if (ext == ".MTLGRAPH")
				{
					file_node->FileType = FileType::MtlGraph;
					file_node->Icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/file_mtlgraph.png"));
				}
				else
				{
					file_node->FileType = FileType::Default;
					file_node->Icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/file.png"));
				}

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

			const auto file_ext = ExtractFileSuffix(child_node->FileName);

			if (child_node->FileType == FileType::Folder) /* 文件夹 */
			{
				/* 文件节点 */
				bool open = ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth);

				/* 文件类型 */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(100);
				ImGui::TextUnformatted(file_ext.c_str());

				/* 文件大小 */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(300);
				ImGui::TextUnformatted("");

				/* 展开文件夹节点 */
				if (open)
				{
					BuildFileUIListTreeDetail(child_node);
					ImGui::TreePop();
				}
			}
			else
			{
				/* 文件节点 */
				ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

				/* todo: 单击文件时的响应 */
				if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen())
				{
					/**/
				}

				/* 右键选项操作 */
				if (ImGui::BeginPopupContextItem())
				{
					/* 在系统文件夹中显示 */
					if (ImGui::MenuItem("Show in file explorer"))
					{
						auto path = g_AssetsPath / child_node->FilePath;
						path = absolute(path).make_preferred();
						OpenFileExplorer(path.string().c_str());
					}

					ImGui::EndPopup();
				}

				/* 文件类型 */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(160);
				ImGui::TextUnformatted(file_ext.c_str());

				/* 文件大小 */
				ImGui::TableNextColumn();
				ImGui::SetNextItemWidth(300);
				ImGui::TextUnformatted((ToString(child_node->FileSize, 2) + "KB").c_str());
			}
		}
	}

	void EditorResourceBrowser::BuildFileUIListTreeSimple(const SharedPtr<FileNode>& node)
	{
		for (const auto& child_node : node->ChildNodes)
		{
			if (child_node->FileType == FileType::Folder) /* 文件夹 */
			{
				/* 文件节点 */
				bool open = ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth);

				/* 点击目录时，设置为当前文件目录 */
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					m_CurrentFileNode = child_node;

				/* 展开文件夹节点 */
				if (open)
				{
					BuildFileUIListTreeSimple(child_node);
					ImGui::TreePop();
				}
			}
			else
			{
				/* 文件节点 */
				ImGui::TreeNodeEx(child_node->FileName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

				/* todo: 单击文件时的响应 */
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