#include "Pch.h"
#include "ModelEditor.h"
#include <glm/gtc/type_ptr.hpp>

namespace Wuya
{
	ModelEditor::ModelEditor()
		: ILayer("ModelEditor")
	{
		PROFILE_FUNCTION();
	}

	void ModelEditor::OnAttached()
	{
		PROFILE_FUNCTION();

		m_pEditorCamera = CreateUniquePtr<EditorCamera>();
		m_pDefaultScene = CreateSharedPtr<Scene>();
		m_pModelInfo = CreateUniquePtr<ModelInfo>();
		m_pMaterialGroup = CreateSharedPtr<MaterialGroup>();

		/* 默认在场景中增加一盏方向光，光源颜色为白色 */
		Entity entity = m_pDefaultScene->CreateEntity("DirectionalLight");
		auto& light_component = entity.AddComponent<LightComponent>(LightType::Directional);
		light_component.Light->SetColor(glm::vec4(1, 1, 1, 1));
		light_component.Light->SetIntensity(1);
		auto& light_transform = entity.GetComponent<TransformComponent>();
		light_transform.Rotation = glm::vec3(0, 0, PI/2);
	}

	void ModelEditor::OnDetached()
	{
		PROFILE_FUNCTION();

		ILayer::OnDetached();
	}

	void ModelEditor::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();
		
		Renderer::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Renderer::Clear();

		m_pEditorCamera->OnUpdate(delta_time);
		m_pDefaultScene->OnUpdateEditor(m_pEditorCamera.get(), delta_time);
	}

	void ModelEditor::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* 显示菜单栏UI */
		ShowMenuUI();
		/* 显示模型编辑UI */
		ShowModelParamsUI();
		/* 资源管理窗口 */
		m_ResourceBrowser.OnImGuiRenderer();
		/* 显示主场景视口 */
		ShowSceneViewportUI();
		/* 材质图编辑器 */
		m_MaterialGraphEditor.OnImGuiRenderer();

		/*bool open = true;
		ImGui::ShowDemoWindow(&open);*/
	}

	void ModelEditor::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		if (!event)
			return;

		m_pEditorCamera->OnEvent(event);
	}

	/* 显示菜单栏UI */
	void ModelEditor::ShowMenuUI()
	{
		PROFILE_FUNCTION();

		static bool p_open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Model Helper", &p_open, window_flags);
		{
			if (!opt_padding)
				ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowMinSize.x = 200.0f;

			/* 菜单栏 */
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Import Model(.obj)", "Ctrl+I"))
					{
						ImportModel();
					}

					if (ImGui::MenuItem("Export Mesh & Mtl(.mesh & .mtl)", "Ctrl+O"))
					{
						ExportMeshAndMtl();
					}

					if (ImGui::MenuItem("Exit"))
						Application::Instance()->Close();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Options"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
					ImGui::MenuItem("Padding", NULL, &opt_padding);
					ImGui::Separator();

					if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
					if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
					if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
					if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
					if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
					ImGui::Separator();

					if (ImGui::MenuItem("Close", NULL, false))
						p_open = false;
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("MaterialGraph"))
				{
					if (ImGui::MenuItem("Material Graph Editor", "Ctrl+M"))
					{
						m_MaterialGraphEditor.ShowOrHide();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	/* 显示主场景视口 */
	void ModelEditor::ShowSceneViewportUI()
	{
		PROFILE_FUNCTION();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");
		{
			/* 获取窗口范围 */
			const auto viewport_region_min = ImGui::GetWindowContentRegionMin();
			const auto viewport_region_max = ImGui::GetWindowContentRegionMax();
			const auto viewport_offset = ImGui::GetWindowPos();
			m_ViewportRegion.MinX = viewport_region_min.x + viewport_offset.x;
			m_ViewportRegion.Width = viewport_region_max.x - viewport_region_min.x;
			m_ViewportRegion.MinY = viewport_region_min.y + viewport_offset.y;
			m_ViewportRegion.Height = viewport_region_max.y - viewport_region_min.x;

			m_pEditorCamera->SetViewportRegion({ 0,0,m_ViewportRegion.Width, m_ViewportRegion.Height });

			///* 若当前ImGui窗口不是主窗口，应阻塞事件传递 */
			//m_IsViewportFocused = ImGui::IsWindowFocused();
			//m_IsViewportHovered = ImGui::IsWindowHovered();
			//Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

			/* 绘制场景 */
			auto output_rt = m_pEditorCamera->GetRenderView()->GetRenderTarget();
			if (output_rt)
			{
				const uint64_t texture_id = output_rt->GetTextureID();
				const auto viewport_panel_size = ImGui::GetContentRegionAvail();
				ImGui::Image((ImTextureID)texture_id, viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}

			/* 拖动资源到主窗口 */
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OnDragItemToScene(g_AssetsPath / path);
				}
				ImGui::EndDragDropTarget();
			}

			///* Gizmos */
			//ShowOperationGizmoUI();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	/* 显示模型编辑UI */
	void ModelEditor::ShowModelParamsUI()
	{
		PROFILE_FUNCTION();

		if (m_pModelInfo->m_SubModelInfos.empty())
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ModelHelper");
		{
			const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			
			/* 显示模型基本信息 */
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				//float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx(ExtractFilename(m_pModelInfo->m_Path).c_str(), flags);
				ImGui::PopStyleVar();

				/* 展开时显示组件内容 */
				if (open)
				{
					/* 基本信息 */
					ImGui::Text("Base Info:");

					{
						ImGui::BulletText("Path: %s", m_pModelInfo->m_Path.c_str());
						const auto& aabb_min = m_pModelInfo->m_AABB.first;
						const auto& aabb_max = m_pModelInfo->m_AABB.second;
						ImGui::BulletText("AABB Min: (%f, %f, %f)", aabb_min.x, aabb_min.y, aabb_min.z);
						ImGui::BulletText("AABB Max: (%f, %f, %f)", aabb_max.x, aabb_max.y, aabb_max.z);
						ImGui::BulletText("Total Vertex Cnt: %llu", m_pModelInfo->m_Attributes.vertices.size() / 3);
						ImGui::BulletText("MeshSegment Cnt: %llu", m_pModelInfo->m_SubModelInfos.size());
					}

					/* 显示各子模型信息 */
					for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
					{
						auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];
						auto& material = m_pMaterialGroup->GetMaterialByIndex(i);

						ImGui::Separator();

						if (ImGui::TreeNode(sub_model_info->Name.empty() ? "Unnamed" : sub_model_info->Name.c_str()))
						{
							/* 基本信息 */
							ImGui::Text("Base Info:");
							{
								const auto& aabb_min = sub_model_info->AABB.first;
								const auto& aabb_max = sub_model_info->AABB.second;
								ImGui::BulletText("AABB Min: (%f, %f, %f)", aabb_min.x, aabb_min.y, aabb_min.z);
								ImGui::BulletText("AABB Max: (%f, %f, %f)", aabb_max.x, aabb_max.y, aabb_max.z);
								ImGui::BulletText("Vertex Cnt: %d", sub_model_info->VertexArray->GetVertexCount());
							}

							/* 材质参数信息 */
							auto& parameters = material->GetAllParameters();
							for (auto& param : parameters)
							{
								auto& param_info = param.second;

								if (ImGui::TreeNode(param_info.Name.c_str()))
								{
									/* 参数类型 */
									const char* data_types[] = { "Texture", "Int", "Float", "Vec2", "Vec3", "Vec4" };
									int current_type_idx = static_cast<int>(param_info.Type);
									const char* preview_value = data_types[current_type_idx];
									if (ImGui::BeginCombo("DataType", preview_value))
									{
										for (int selected_idx = 0; selected_idx < IM_ARRAYSIZE(data_types); ++selected_idx)
										{
											/* 切换数据类型时，设置新类型的默认值 */
											const bool is_selected = (current_type_idx == selected_idx);
											if (ImGui::Selectable(data_types[selected_idx], is_selected))
											{
												current_type_idx = selected_idx;
												auto param_type = static_cast<ParamType>(selected_idx);
												switch (param_type)
												{
												case ParamType::Texture:
													{
														auto GetSlot = [](const std::string& name)
														{
															if (strcmp(name.c_str(), "Ambient") == 0)
																return TextureSlot::Ambient;
															if (strcmp(name.c_str(), "Albedo") == 0)
																return TextureSlot::Albedo;
															if (strcmp(name.c_str(), "Specular") == 0)
																return TextureSlot::Specular;
															if (strcmp(name.c_str(), "Metallic") == 0)
																return TextureSlot::Metallic;
															if (strcmp(name.c_str(), "Roughness") == 0)
																return TextureSlot::Roughness;
															if (strcmp(name.c_str(), "Emissive") == 0)
																return TextureSlot::Emissive;
															if (strcmp(name.c_str(), "Normal") == 0)
																return TextureSlot::Normal;
															if (strcmp(name.c_str(), "Bump") == 0)
																return TextureSlot::Bump;
															if (strcmp(name.c_str(), "Displacement") == 0)
																return TextureSlot::Displacement;
															return TextureSlot::Invalid;
														};
														/* 填入默认值 */
														const auto& default_texture = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Default.png"));
														material->SetTexture(param_info.Name, default_texture, GetSlot(param_info.Name));
													}
													break;
												case ParamType::Int:
													material->SetParameters(ParamType::Int, param_info.Name, 0);
													break;
												case ParamType::Float: 
													material->SetParameters(ParamType::Float, param_info.Name, 0.0f);
													break;
												case ParamType::Vec2: 
													material->SetParameters(ParamType::Vec2, param_info.Name, glm::vec2(0.0f));
													break;
												case ParamType::Vec3: 
													material->SetParameters(ParamType::Vec3, param_info.Name, glm::vec3(0.0f));
													break;
												case ParamType::Vec4: 
													material->SetParameters(ParamType::Vec4, param_info.Name, glm::vec4(0.0f));
													break;
												}
											}

											// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
											if (is_selected)
												ImGui::SetItemDefaultFocus();
										}
										ImGui::EndCombo();
									}

									/* 显示当前参数 */
									switch (param_info.Type)
									{
									case ParamType::Texture:
										{
											auto texture_info = std::any_cast<std::pair<SharedPtr<Texture>, uint32_t>>(param_info.Value);
											auto& texture = texture_info.first;
											auto load_config = texture->GetTextureLoadConfig();

											/* Show texture image */
											ImGui::PushID(param_info.Name.c_str());
											ImGui::Columns(2);

											ImGui::SetColumnWidth(0, 100);
											ImGui::ImageButton((ImTextureID)texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
											if (ImGui::BeginDragDropTarget())
											{
												if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
												{
													const wchar_t* path = (const wchar_t*)payload->Data;
													const std::filesystem::path texture_path = path;

													material->SetTexture(param_info.Name, TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(texture_path), load_config), texture_info.second);
													sub_model_info->MaterialParams.AmbientTexPath = { texture_path.string(), true };
												}
												ImGui::EndDragDropTarget();
											}

											/* Hovered */
											if (ImGui::IsItemHovered())
											{
												ImGui::BeginTooltip();
												ImGui::Image((ImTextureID)texture->GetTextureID(), ImVec2(240, 240), ImVec2(0, 1), ImVec2(1, 0));
												ImGui::EndTooltip();
											}

											ImGui::NextColumn();

											/* Show texture path */
											ImGui::TextWrapped(texture->GetPath().c_str());
											/* Show texture load config */
											ImGui::Checkbox("IsFlipV", &load_config.IsFlipV);
											ImGui::Checkbox("IsGenMips", &load_config.IsGenMips);

											/* 加载选项发生改动时，立即重新加载贴图 */
											if (load_config != texture->GetTextureLoadConfig())
											{
												auto new_texture = TextureAssetManager::Instance().GetOrCreateTexture(texture->GetPath(), load_config);
												material->SetTexture(param_info.Name, new_texture, texture_info.second);
											}

											ImGui::Columns(1);
											ImGui::PopID();
										}
										break;
									case ParamType::Int:
										{
											ImGui::PushID(param_info.Name.c_str());

											/* Show int controller */
											int value = std::any_cast<int>(param.second.Value);
											ImGui::DragInt(param_info.Name.c_str(), &value);
											material->SetParameters(ParamType::Int, param_info.Name, value);

											ImGui::PopID();
										}
										break;
									case ParamType::Float:
										{
											ImGui::PushID(param_info.Name.c_str());

											/* Show float controller */
											float value = std::any_cast<float>(param.second.Value);
											ImGui::DragFloat(param_info.Name.c_str(), &value);
											material->SetParameters(ParamType::Float, param_info.Name, value);

											ImGui::PopID();
										}
										break;
									case ParamType::Vec2:
										{
											ImGui::PushID(param_info.Name.c_str());

											/* Show glm::vec2 controller */
											glm::vec2 value = std::any_cast<glm::vec2>(param.second.Value);
											ImGui::DragFloat2(param_info.Name.c_str(), glm::value_ptr(value));
											material->SetParameters(ParamType::Vec2, param_info.Name, value);

											ImGui::PopID();
										}
										break;
									case ParamType::Vec3:
										{
											ImGui::PushID(param_info.Name.c_str());

											/* Show glm::vec3 controller */
											glm::vec3 value = std::any_cast<glm::vec3>(param.second.Value);
											ImGui::DragFloat3(param_info.Name.c_str(), glm::value_ptr(value));
											material->SetParameters(ParamType::Vec3, param_info.Name, value);

											ImGui::PopID();
										}
										break;
									case ParamType::Vec4:
										{
											ImGui::PushID(param_info.Name.c_str());

											/* Show glm::vec4 controller */
											glm::vec4 value = std::any_cast<glm::vec4>(param.second.Value);
											ImGui::DragFloat4(param_info.Name.c_str(), glm::value_ptr(value));
											material->SetParameters(ParamType::Vec4, param_info.Name, value);

											ImGui::PopID();
										}
										break;
									}

									ImGui::TreePop();
								}
							}

							ImGui::TreePop();
						}
					}

					/*for (const auto& mesh_segment : m_pModel->GetMeshSegments())
					{
						if (ImGui::CollapsingHeader(mesh_segment->GetDebugName().empty() ? "Unnamed" : mesh_segment->GetDebugName().c_str()))
						{
							const auto& aabb_min = mesh_segment->GetAABBMin();
							const auto& aabb_max = mesh_segment->GetAABBMax();
							ImGui::BulletText("AABB Min: (%f, %f, %f)", aabb_min.x, aabb_min.y, aabb_min.z);
							ImGui::BulletText("AABB Max: (%f, %f, %f)", aabb_max.x, aabb_max.y, aabb_max.z);
							ImGui::BulletText("Vertex Cnt: %d", mesh_segment->GetVertexArray()->GetVertexCount());
							const auto& material = mesh_segment->GetMaterial();
							ImGui::BulletText("Material Path: %s", material->GetShader()->GetPath().c_str());
							ImGui::Separator();

							for (uint8_t slot = 0; slot < TextureSlot::ValidSlotCnt; ++slot)
							{
								SharedPtr<Texture> tex = material->GetTextureBySlot(slot);
								float factor = 1.0f;
								if (ImGui::TreeNode("Test"))
								{
									PackedUIFuncs::DrawTextureUI(TextureSlot::GetSlotName(slot), tex, factor);
									material->SetTexture(tex, slot);
									ImGui::TreePop();
								}
							}
						}
						ImGui::Separator();
					}*/

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	/* 响应拖拽文件到主窗口 */
	void ModelEditor::OnDragItemToScene(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		const auto extension = path.extension().string();

		/* 模型文件 */
		if (extension == ".obj" || extension == ".fbx")
		{
			EDITOR_LOG_DEBUG("Import model file: {}.", path.generic_string());

			/* 读取模型 */
			m_pModelInfo->Reset();
			m_pModelInfo->LoadFromPath(path.generic_string());

			/* 初始化材质 */
			m_pMaterialGroup->ClearAllMaterials();
			for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
				m_pMaterialGroup->EmplaceMaterial(Material::Create(ShaderAssetManager::Instance().GetOrLoad(ABSOLUTE_PATH("Shaders/default.glsl"))));

			/* 更新场景模型信息 */
			UpdateModel();

			return;
		}

		if (extension == ".mtlgraph")
		{
			EDITOR_LOG_DEBUG("Import material graph file: {}.", path.generic_string());

			m_MaterialGraphEditor.Deserializer(path.generic_string());
		}
	}

	/* 导入模型 */
	void ModelEditor::ImportModel()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("Obj(*.obj)\0*.obj\0FBX(*.fbx)\0*.fbx\0");
		if (!file_path.empty())
		{
			/* 读取模型 */
			m_pModelInfo->Reset();
			m_pModelInfo->LoadFromPath(file_path);

			/* 初始化材质 */
			m_pMaterialGroup->ClearAllMaterials();
			for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
				m_pMaterialGroup->EmplaceMaterial(Material::Create(ShaderAssetManager::Instance().GetOrLoad(ABSOLUTE_PATH("Shaders/default.glsl"))));

			/* 更新场景模型信息 */
			UpdateModel();
		}
	}

	/* 拷贝指定格式的文件到目标路径 */
	static bool CopyFileFromTo(const std::filesystem::path& src_path, const std::filesystem::path& dst_path, const std::regex& suffix)
	{
		PROFILE_FUNCTION();

		if (src_path.empty() || src_path.empty())
			return false;

		if (!std::filesystem::exists(src_path))
			return false;

		for (auto& item : std::filesystem::directory_iterator(src_path))
		{
			std::filesystem::path dst_item_path = dst_path / item.path().filename();
			if (std::filesystem::is_directory(item.status()))
			{
				_mkdir(dst_item_path.string().c_str());
				CopyFileFromTo(item.path(), dst_item_path, suffix);
			}
			else
			{
				if (std::regex_match(item.path().extension().string(), suffix))
				{
					if (item.path() != dst_item_path)
						std::filesystem::copy_file(item.path(), dst_item_path, std::filesystem::copy_options::skip_existing);
				}
			}
		}
		return true;
	}

	/* 导出模型 */
	void ModelEditor::ExportMeshAndMtl()
	{
		PROFILE_FUNCTION();

		if (!m_pModelInfo || !m_pModel)
			return;

		const auto file_path = FileDialog::SaveFile("Mesh(*.mesh)\0*.mesh\0");
		if (!file_path.empty())
		{
			/* 导出模型 */
			ExportMesh(file_path);

			/* 导出材质 */
			const auto& material_path = ReplaceFileSuffix(file_path, ".mtl");
			m_pMaterialGroup->Serializer(material_path);

			/* 拷贝贴图文件 */
			std::filesystem::path current_model_path(m_pModelInfo->m_Path);
			std::filesystem::path target_path(file_path);
			const std::regex pattern("^[\s\S]*\.(pdf|png|jpeg|jpg|tga|bmp|dds)$");
			CopyFileFromTo(current_model_path.parent_path(), target_path.parent_path(), pattern);
		}
	}

	/* 导出Mesh */
	void ModelEditor::ExportMesh(const std::string& path)
	{
		PROFILE_FUNCTION();
		
		std::ofstream out_mesh_file(path, std::ios::out | std::ios::binary);

		/* 写入子模型数量: size_t * 1 */
		size_t sub_model_count = m_pModelInfo->m_SubModelInfos.size();
		out_mesh_file.write((char*)&sub_model_count, sizeof(size_t));

		/* 逐个写入子模型信息 */
		for (int i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
		{
			const auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];

			/* 写入Name的size: size_t * 1 */
			size_t name_size = sub_model_info->Name.size();
			out_mesh_file.write((char*)&name_size, sizeof(size_t));

			/* 写入Name内容: name_size */
			out_mesh_file.write(sub_model_info->Name.c_str(), name_size);

			/* 写入顶点数量: uint32_t */
			out_mesh_file.write((char*)&(sub_model_info->VertexCount), sizeof(uint32_t));

			/*先写Indices信息 */
			{
				/* 写入Indices的size: size_t * 1 */
				size_t count = sub_model_info->Indices.size();
				out_mesh_file.write((char*)&count, sizeof(size_t));
				
				/* 写入Indices内容: data_size */
				out_mesh_file.write((char*)(sub_model_info->Indices.data()), count * sizeof(uint32_t));
			}

			const auto& vertex_buffers = sub_model_info->VertexArray->GetVertexBuffers();
			/* 写入VertexBuffer的数量: size_t * 1 */
			size_t vertex_buffer_count = vertex_buffers.size();
			out_mesh_file.write((char*)&vertex_buffer_count, sizeof(size_t));

			/* 再写入每个VertexData信息 */
			for (size_t i = 0; i < vertex_buffer_count; ++i)
			{
				const auto& buffer_data = sub_model_info->VertexBufferDatas[i];
				
				/* 写入该data具有多少float: uint32_t * 1 */
				out_mesh_file.write((char*)&(buffer_data.first), sizeof(uint32_t));
				/* 写入顶点数据: data_size */
				out_mesh_file.write((char*)(buffer_data.second), sub_model_info->VertexCount * buffer_data.first * sizeof(float));

				/* 写入VertexLayout */
				const auto& vertex_buffer = vertex_buffers[i];
				const auto& elements = vertex_buffer->GetLayout().GetElements();

				/* 写入 VertexLayout的Element数量: size_t * 1 */
				size_t element_count = elements.size();
				out_mesh_file.write((char*)&element_count, sizeof(size_t));
				/* 逐个写入Element信息: */
				for (auto& element : elements)
				{
					/* 写入Name的size: size_t * 1 */
					size_t element_name_size = element.Name.size();
					out_mesh_file.write((char*)&element_name_size, sizeof(size_t));
					/* 写入Name内容：element_name_size */
					out_mesh_file.write(element.Name.c_str(), element_name_size);
					/* 写入Element的类型: uint8_t * 1 */
					out_mesh_file.write((char*)&element.Type, sizeof(uint8_t));
					/* 写入Element的偏移：size_t * 1 */
					out_mesh_file.write((char*)&element.Offset, sizeof(size_t));
					/* 写入Element的Normalized: bool * 1 */
					out_mesh_file.write((char*)&element.Normalized, sizeof(bool));
				}
			}

			/* 写入材质索引：int * 1 */
			auto material = m_pMaterialGroup->GetMaterialByIndex(i);
			int material_id = -1;
			for (int i = 0; i < m_pMaterialGroup->GetAllMaterials().size(); ++i)
			{
				const auto& mtl = m_pMaterialGroup->GetMaterialByIndex(i);
				if (mtl == material)
				{
					material_id = i;
					break;
				}
			}
			out_mesh_file.write((char*)&material_id, sizeof(int));

			/* 写入aabb: sizeof(glm::vec3) * 2 */
			out_mesh_file.write((char*)&sub_model_info->AABB.first, sizeof(glm::vec3));
			out_mesh_file.write((char*)&sub_model_info->AABB.second, sizeof(glm::vec3));
		}

		/* 保存Mesh文件 */
		out_mesh_file.close();
	}

	/* 更新模型 */
	void ModelEditor::UpdateModel()
	{
		PROFILE_FUNCTION();

		/* 先移除场景中的其他模型 */
		m_pDefaultScene->DestroyTargetEntities<ModelComponent>();

		/* 新建模型 */
		m_pModel = CreateSharedPtr<Model>(m_pModelInfo->m_Path);
		for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
		{
			auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];
			auto& material = m_pMaterialGroup->GetMaterialByIndex(i);

			/* 更新材质 */
			UpdateMaterial(material, sub_model_info->MaterialParams);
			
			SharedPtr<MeshSegment> mesh_segment = CreateSharedPtr<MeshSegment>(sub_model_info->Name, sub_model_info->VertexArray, material);
			mesh_segment->SetAABB(sub_model_info->AABB.first, sub_model_info->AABB.second);

			m_pModel->AddMeshSegment(mesh_segment);
		}

		/* 将模型添加到场景中 */
		Entity entity = m_pDefaultScene->CreateEntity(m_pModel->GetDebugName());
		auto& mesh_component = entity.AddComponent<ModelComponent>();
		mesh_component.Model = m_pModel;

		/* 根据模型大小自适应相机距离 */
		const auto& aabb_min = m_pModel->GetAABBMin();
		const auto& aabb_max = m_pModel->GetAABBMax();
		const auto aabb_height = aabb_max.y - aabb_min.y;
		const auto distance = (aabb_height) / std::tanf(m_pEditorCamera->GetFov() / 2.0f);
		auto aabb_half_range = (aabb_max - aabb_min) * 0.5f;
		float radius = std::sqrt(aabb_half_range.x * aabb_half_range.x + aabb_half_range.y * aabb_half_range.y + aabb_half_range.z * aabb_half_range.z);
		m_pEditorCamera->SetDistance(distance);
		m_pEditorCamera->SetFarClip(radius + distance);
		m_pEditorCamera->SetFocalPoint((aabb_min + aabb_max) * 0.5f);
	}

	/* 更新材质：根据材质参数设置材质 */
	void ModelEditor::UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params)
	{
		PROFILE_FUNCTION();

		TextureLoadConfig load_config;
		load_config.IsFlipV = false;
		// load_config.SamplerWrapMode = SamplerWrapMode::ClampToEdge;
		load_config.SamplerMinFilter = SamplerMinFilter::LinearMipmapLinear;
		{
			/* Ambient */
			if (material_params.AmbientTexPath.second)
				material->SetTexture("AmbientTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.AmbientTexPath.first), load_config), TextureSlot::Ambient);
			else
				material->SetTexture("AmbientTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Ambient);
			if (material_params.AmbientFactor.second)
				material->SetParameters(ParamType::Vec3, "AmbientColor", material_params.AmbientFactor.first);

			/* Diffuse/Albedo */
			if (material_params.DiffuseTexPath.second)
				material->SetTexture("AlbedoTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.DiffuseTexPath.first), load_config), TextureSlot::Albedo);
			else
				material->SetTexture("AlbedoTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/White.png"), load_config), TextureSlot::Albedo);
			if (material_params.DiffuseFactor.second)
				material->SetParameters(ParamType::Vec3, "AlbedoColor", material_params.DiffuseFactor.first);

			/* Specular */
			if (material_params.SpecularTexPath.second)
				material->SetTexture("SpecularTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.SpecularTexPath.first), load_config), TextureSlot::Specular);
			else
				material->SetTexture("SpecularTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Specular);
			if (material_params.SpecularFactor.second)
				material->SetParameters(ParamType::Vec3, "SpecularColor", material_params.SpecularFactor.first);

			/* Normal, todo: 处理Bump和Displacement */
			if (material_params.NormalTexPath.second)
				material->SetTexture("NormalTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.NormalTexPath.first), load_config), TextureSlot::Normal);
			else
				material->SetTexture("NormalTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Normal.png"), load_config), TextureSlot::Normal);
			if (material_params.BumpTexPath.second)
				material->SetTexture("BumpTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.BumpTexPath.first), load_config), TextureSlot::Bump);
			else
				material->SetTexture("BumpTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Bump);
			if (material_params.DisplacementTexPath.second)
				material->SetTexture("DisplacementTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.DisplacementTexPath.first), load_config), TextureSlot::Displacement);
			else
				material->SetTexture("DisplacementTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Displacement);
			if (!material_params.NormalTexPath.second && !material_params.BumpTexPath.second && !material_params.DisplacementTexPath.second)
				material->SetTexture("NormalTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Normal.png"), load_config), TextureSlot::Normal);

			/* Roughness */
			if (material_params.RoughnessTexPath.second)
				material->SetTexture("RoughnessTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.RoughnessTexPath.first), load_config), TextureSlot::Roughness);
			else
				material->SetTexture("RoughnessTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/White.png"), load_config), TextureSlot::Roughness);
			if (material_params.RoughnessFactor.second)
				material->SetParameters(ParamType::Float, "Roughness", material_params.RoughnessFactor.first);

			/* Metallic */
			if (material_params.MetallicTexPath.second)
				material->SetTexture("MetallicTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.MetallicTexPath.first), load_config), TextureSlot::Metallic);
			else
				material->SetTexture("MetallicTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Metallic);
			if (material_params.MetallicFactor.second)
				material->SetParameters(ParamType::Float, "Metallic", material_params.MetallicFactor.first);

			/* Emission */
			if (material_params.EmissionTexPath.second)
				material->SetTexture("EmissiveTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH(material_params.EmissionTexPath.first), load_config), TextureSlot::Emissive);
			else
				material->SetTexture("EmissiveTex", TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("Textures/Black.png"), load_config), TextureSlot::Emissive);
			if (material_params.EmissionFactor.second)
				material->SetParameters(ParamType::Vec3, "EmissiveColor", material_params.EmissionFactor.first);

			/* ClearCoat */
			if (material_params.ClearCoatRoughness.second)
				material->SetParameters(ParamType::Float, "ClearCoatRoughness", material_params.ClearCoatRoughness.first);
			if (material_params.ClearCoatThickness.second)
				material->SetParameters(ParamType::Float, "ClearCoatThickness", material_params.ClearCoatThickness.first);

			/* Others */
			if (material_params.TransmittanceColor.second)
				material->SetParameters(ParamType::Vec3, "TransmittanceColor", material_params.TransmittanceColor.first);
			if (material_params.IOR.second)
				material->SetParameters(ParamType::Float, "IOR", material_params.IOR.first);
		}
	}
}
