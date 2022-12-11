#include "Pch.h"
#include "ModelEditorLayer.h"
#include <glm/gtc/type_ptr.hpp>

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	ModelEditorLayer::ModelEditorLayer()
		: ILayer("ModelEditorLayer")
	{
		PROFILE_FUNCTION();
	}

	void ModelEditorLayer::OnAttached()
	{
		PROFILE_FUNCTION();

		m_pEditorCamera = CreateUniquePtr<EditorCamera>();
		m_pDefaultScene = CreateSharedPtr<Scene>();
		m_pModelInfo = CreateUniquePtr<ModelInfo>();
		m_pMaterialGroup = CreateSharedPtr<MaterialGroup>();

		/* Ĭ���ڳ���������һյ����⣬��Դ��ɫΪ��ɫ */
		Entity entity = m_pDefaultScene->CreateEntity("DirectionalLight");
		auto& light_component = entity.AddComponent<LightComponent>(LightType::Directional);
		light_component.Light->SetColor(glm::vec4(1, 1, 1, 1));
	}

	void ModelEditorLayer::OnDetached()
	{
		PROFILE_FUNCTION();

		ILayer::OnDetached();
	}

	void ModelEditorLayer::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();
		
		Renderer::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Renderer::Clear();

		m_pEditorCamera->OnUpdate(delta_time);
		m_pDefaultScene->OnUpdateEditor(m_pEditorCamera.get(), delta_time);
	}

	void ModelEditorLayer::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* ��ʾ�˵���UI */
		ShowMenuUI();
		/* ��ʾģ�ͱ༭UI */
		ShowModelParamsUI();
		/* ��Դ������ */
		m_ResourceBrowser.OnImGuiRenderer();
		/* ��ʾ�������ӿ� */
		ShowSceneViewportUI();
		/*bool open = true;
		ImGui::ShowDemoWindow(&open);*/
	}

	void ModelEditorLayer::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		if (!event)
			return;

		m_pEditorCamera->OnEvent(event);
	}

	/* ��ʾ�˵���UI */
	void ModelEditorLayer::ShowMenuUI()
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

			/* �˵��� */
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

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	/* ��ʾ�������ӿ� */
	void ModelEditorLayer::ShowSceneViewportUI()
	{
		PROFILE_FUNCTION();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");
		{
			/* ��ȡ���ڷ�Χ */
			const auto viewport_region_min = ImGui::GetWindowContentRegionMin();
			const auto viewport_region_max = ImGui::GetWindowContentRegionMax();
			const auto viewport_offset = ImGui::GetWindowPos();
			m_ViewportRegion.MinX = viewport_region_min.x + viewport_offset.x;
			m_ViewportRegion.Width = viewport_region_max.x - viewport_region_min.x;
			m_ViewportRegion.MinY = viewport_region_min.y + viewport_offset.y;
			m_ViewportRegion.Height = viewport_region_max.y - viewport_region_min.x;

			m_pEditorCamera->SetViewportRegion({ 0,0,m_ViewportRegion.Width, m_ViewportRegion.Height });

			///* ����ǰImGui���ڲ��������ڣ�Ӧ�����¼����� */
			//m_IsViewportFocused = ImGui::IsWindowFocused();
			//m_IsViewportHovered = ImGui::IsWindowHovered();
			//Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

			/* ���Ƴ��� */
			auto output_rt = m_pEditorCamera->GetRenderView()->GetRenderTarget();
			if (output_rt)
			{
				const uint64_t texture_id = output_rt->GetTextureID();
				const auto viewport_panel_size = ImGui::GetContentRegionAvail();
				ImGui::Image((ImTextureID)texture_id, viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}

			/* �϶���Դ�������� */
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path absolute_path = absolute(g_AssetPath);
					OnDragItemToScene(g_AssetPath / path);
				}
				ImGui::EndDragDropTarget();
			}

			///* Gizmos */
			//ShowOperationGizmoUI();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	/* ��ʾģ�ͱ༭UI */
	void ModelEditorLayer::ShowModelParamsUI()
	{
		PROFILE_FUNCTION();

		if (m_pModelInfo->m_SubModelInfos.empty())
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ModelHelper");
		{
			const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			
			/* ��ʾģ�ͻ�����Ϣ */
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				//float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx(ExtractFilename(m_pModelInfo->m_Path).c_str(), flags);
				ImGui::PopStyleVar();

				/* չ��ʱ��ʾ������� */
				if (open)
				{
					/* ������Ϣ */
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

					/* ��ʾ����ģ����Ϣ */
					for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
					{
						auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];
						auto& material = m_pMaterialGroup->GetMaterialByIndex(i);

						ImGui::Separator();

						if (ImGui::TreeNode(sub_model_info->Name.empty() ? "Unnamed" : sub_model_info->Name.c_str()))
						{
							/* ������Ϣ */
							ImGui::Text("Base Info:");
							{
								const auto& aabb_min = sub_model_info->AABB.first;
								const auto& aabb_max = sub_model_info->AABB.second;
								ImGui::BulletText("AABB Min: (%f, %f, %f)", aabb_min.x, aabb_min.y, aabb_min.z);
								ImGui::BulletText("AABB Max: (%f, %f, %f)", aabb_max.x, aabb_max.y, aabb_max.z);
								ImGui::BulletText("Vertex Cnt: %d", sub_model_info->VertexArray->GetVertexCount());
							}

							/* ���ʲ�����Ϣ */
							auto& parameters = material->GetAllParameters();
							for (auto& param : parameters)
							{
								auto& param_info = param.second;

								if (ImGui::TreeNode(param_info.Name.c_str()))
								{
									/* �������� */
									const char* data_types[] = { "Texture", "Int", "Float", "Vec2", "Vec3", "Vec4" };
									int current_type_idx = static_cast<int>(param_info.Type);
									const char* preview_value = data_types[current_type_idx];
									if (ImGui::BeginCombo("DataType", preview_value))
									{
										for (int selected_idx = 0; selected_idx < IM_ARRAYSIZE(data_types); ++selected_idx)
										{
											/* �л���������ʱ�����������͵�Ĭ��ֵ */
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
															return TextureSlot::Invalid;
														};
														/* ����Ĭ��ֵ */
														const auto& default_texture = TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/default_texture.png", {});
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

									/* ��ʾ��ǰ���� */
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

													material->SetTexture(param_info.Name, TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / texture_path).generic_string(), load_config), texture_info.second);
													sub_model_info->MaterialParams.AmbientTexPath = texture_path.string();
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

											/* ����ѡ����Ķ�ʱ���������¼�����ͼ */
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

	/* ��Ӧ��ק�ļ��������� */
	void ModelEditorLayer::OnDragItemToScene(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		const auto extension = path.extension().string();

		/* ģ���ļ� */
		if (extension == ".obj")
		{
			EDITOR_LOG_DEBUG("Import model file: {}.", path.generic_string());

			/* ��ȡģ�� */
			m_pModelInfo->Reset();
			m_pModelInfo->LoadFromObj(path.generic_string());

			/* ��ʼ������ */
			m_pMaterialGroup->ClearAllMaterials();
			for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
				m_pMaterialGroup->EmplaceMaterial(Material::Create(ShaderAssetManager::Instance().GetOrLoad("assets/shaders/default.glsl")));

			/* ���³���ģ����Ϣ */
			UpdateModel();

			return;
		}
	}

	/* ����ģ�� */
	void ModelEditorLayer::ImportModel()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("Obj(*.obj)\0*.obj\0");
		if (!file_path.empty())
		{
			/* ��ȡģ�� */
			m_pModelInfo->Reset();
			m_pModelInfo->LoadFromObj(file_path);

			/* ��ʼ������ */
			m_pMaterialGroup->ClearAllMaterials();
			for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
				m_pMaterialGroup->EmplaceMaterial(Material::Create(ShaderAssetManager::Instance().GetOrLoad("assets/shaders/default.glsl")));

			/* ���³���ģ����Ϣ */
			UpdateModel();
		}
	}

	/* ����ģ�� */
	void ModelEditorLayer::ExportMeshAndMtl()
	{
		PROFILE_FUNCTION();

		if (!m_pModelInfo || !m_pModel)
			return;

		const auto file_path = FileDialog::SaveFile("Mesh(*.mesh)\0*.mesh\0");
		if (!file_path.empty())
		{
			/* ����ģ�� */
			ExportMesh(file_path);

			/* �������� */
			const auto& material_path = ReplaceFileSuffix(file_path, ".mtl");
			m_pMaterialGroup->Serializer(material_path);

			/* ������ͼ�ļ� */
			std::filesystem::path current_model_path(m_pModelInfo->m_Path);
			std::filesystem::path target_path(file_path);
			const std::regex pattern("^[\s\S]*\.(pdf|png|jpeg|jpg|tga|bmp|dds)$");
			CopyFileFromTo(current_model_path.parent_path(), target_path.parent_path(), pattern);
		}
	}

	void ModelEditorLayer::ExportMesh(const std::string& path)
	{
		PROFILE_FUNCTION();
		
		std::ofstream out_mesh_file(path, std::ios::out | std::ios::binary);

		/* д����ģ������: size_t * 1 */
		size_t sub_model_count = m_pModelInfo->m_SubModelInfos.size();
		out_mesh_file.write((char*)&sub_model_count, sizeof(size_t));

		/* ���д����ģ����Ϣ */
		for (int i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
		{
			const auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];

			/* д��Name��size: size_t * 1 */
			size_t name_size = sub_model_info->Name.size();
			out_mesh_file.write((char*)&name_size, sizeof(size_t));

			/* д��Name���ݣ�name_size */
			out_mesh_file.write(sub_model_info->Name.c_str(), name_size);

			/* д��VertexData�Ĵ�С: size_t * 1 */
			size_t data_size = sub_model_info->VertexData.size();
			out_mesh_file.write((char*)&data_size, sizeof(size_t));

			/* д��VertexData����: data_size */
			out_mesh_file.write((char*)(sub_model_info->VertexData.data()), data_size * sizeof(float));

			/* д��VertexLayout */
			auto& vertex_layout = sub_model_info->VertexArray->GetVertexBuffers()[0]->GetLayout();
			auto& elements = vertex_layout.GetElements();

			/* д�� VertexLayout��Element����: size_t * 1 */
			size_t element_count = elements.size();
			out_mesh_file.write((char*)&element_count, sizeof(size_t));
			/* ���д��Element��Ϣ */
			for (auto& element : elements)
			{
				/* д��Name��size: size_t * 1 */
				size_t element_name_size = element.Name.size();
				out_mesh_file.write((char*)&element_name_size, sizeof(size_t));
				/* д��Name���ݣ�element_name_size */
				out_mesh_file.write(element.Name.c_str(), element_name_size);
				/* д��Element������: uint8_t * 1 */
				out_mesh_file.write((char*)&element.Type, sizeof(uint8_t));
				/* д��Element��ƫ�ƣ�size_t * 1 */
				out_mesh_file.write((char*)&element.Offset, sizeof(size_t));
				/* д��Element��Normalized: bool * 1 */
				out_mesh_file.write((char*)&element.Normalized, sizeof(bool));
			}

			/* д�����������int * 1 */
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

			/* д��aabb: sizeof(glm::vec3) * 2 */
			out_mesh_file.write((char*)&sub_model_info->AABB.first, sizeof(glm::vec3));
			out_mesh_file.write((char*)&sub_model_info->AABB.second, sizeof(glm::vec3));
		}

		/* ����Mesh�ļ� */
		out_mesh_file.close();
	}

	/* ����ģ�� */
	void ModelEditorLayer::UpdateModel()
	{
		PROFILE_FUNCTION();

		/* ���Ƴ������е�����ģ�� */
		m_pDefaultScene->DestroyTargetEntities<ModelComponent>();

		/* �½�ģ�� */
		m_pModel = CreateSharedPtr<Model>(m_pModelInfo->m_Path);
		for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
		{
			auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];
			auto& material = m_pMaterialGroup->GetMaterialByIndex(i);

			/* ���²��� */
			UpdateMaterial(material, sub_model_info->MaterialParams);
			
			SharedPtr<MeshSegment> mesh_segment = CreateSharedPtr<MeshSegment>(sub_model_info->Name, sub_model_info->VertexArray, material);
			mesh_segment->SetAABB(sub_model_info->AABB.first, sub_model_info->AABB.second);

			m_pModel->AddMeshSegment(mesh_segment);
		}

		/* ��ģ����ӵ������� */
		Entity entity = m_pDefaultScene->CreateEntity(m_pModel->GetDebugName());
		auto& mesh_component = entity.AddComponent<ModelComponent>();
		mesh_component.Model = m_pModel;

		/* todo: ����ģ�ʹ�С����Ӧ������� */
		const glm::vec3 center = (m_pModel->GetAABBMin() + m_pModel->GetAABBMax()) * 0.5f;
		auto& transform_component = entity.GetComponent<TransformComponent>();
		transform_component.Position = -center;
		transform_component.Scale = glm::vec3(0.5f);
	}

	/* ���²��ʣ����ݲ��ʲ������ò��� */
	void ModelEditorLayer::UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params)
	{
		PROFILE_FUNCTION();

		TextureLoadConfig load_config;
		load_config.IsFlipV = false;
		{
			/* Ambient */
			if (!material_params.AmbientTexPath.empty())
				material->SetTexture("Ambient", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.AmbientTexPath).generic_string(), load_config), TextureSlot::Ambient);
			else
				material->SetParameters(ParamType::Vec3, "Ambient", material_params.Ambient);

			/* Diffuse/Albedo */
			if (!material_params.DiffuseTexPath.empty())
				material->SetTexture("Albedo", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.DiffuseTexPath).generic_string(), load_config), TextureSlot::Albedo);
			else
				material->SetParameters(ParamType::Vec3, "Albedo", material_params.Diffuse);

			/* Specular */
			if (!material_params.SpecularTexPath.empty())
				material->SetTexture("Specular", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.SpecularTexPath).generic_string(), load_config), TextureSlot::Specular);
			else
				material->SetParameters(ParamType::Vec3, "Specular", material_params.Specular);

			/* Normal, todo: ����Bump��Displacement */
			if (!material_params.BumpTexPath.empty())
				material->SetTexture("Normal", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.BumpTexPath).generic_string(), load_config), TextureSlot::Normal);
			else if (!material_params.DisplacementTexPath.empty())
				material->SetTexture("Normal", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.DisplacementTexPath).generic_string(), load_config), TextureSlot::Normal);
			else
				material->SetParameters(ParamType::Vec3, "Normal", glm::vec3(0.0f, 0.0f, 1.0f));

			/* Roughness */
			if (!material_params.RoughnessTexPath.empty())
				material->SetTexture("Roughness", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.RoughnessTexPath).generic_string(), load_config), TextureSlot::Roughness);
			else
				material->SetParameters(ParamType::Float, "Roughness", material_params.Roughness);

			/* Metallic */
			if (!material_params.MetallicTexPath.empty())
				material->SetTexture("Metallic", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.MetallicTexPath).generic_string(), load_config), TextureSlot::Metallic);
			else
				material->SetParameters(ParamType::Float, "Metallic", material_params.Metallic);

			/* Emission */
			if (!material_params.EmissionTexPath.empty())
				material->SetTexture("Emissive", TextureAssetManager::Instance().GetOrCreateTexture((g_AssetPath / material_params.EmissionTexPath).generic_string(), load_config), TextureSlot::Emissive);
			else
				material->SetParameters(ParamType::Vec3, "Emissive", material_params.Emission);

			/* ClearCoat */
			material->SetParameters(ParamType::Float, "ClearCoatRoughness", material_params.ClearCoatRoughness);
			material->SetParameters(ParamType::Float, "ClearCoatThickness", material_params.ClearCoatThickness);

			/* Others */
			material->SetParameters(ParamType::Vec3, "Transmittance", material_params.Transmittance);
			material->SetParameters(ParamType::Float, "IOR", material_params.IOR);
		}
	}

	bool ModelEditorLayer::CopyFileFromTo(const std::filesystem::path& src_path, const std::filesystem::path& dst_path, const std::regex& suffix)
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
					std::filesystem::copy_file(item.path(), dst_item_path, std::filesystem::copy_options::skip_existing);
				}
			}
		}
	}
}
