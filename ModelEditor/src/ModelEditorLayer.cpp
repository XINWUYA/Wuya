#include "Pch.h"
#include "ModelEditorLayer.h"

namespace Wuya
{
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

		/* 默认在场景中增加一盏方向光，光源颜色为白色 */
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

		/* 显示菜单栏UI */
		ShowMenuUI();
		/* 显示模型编辑UI */
		ShowModelParamsUI();
		/* 显示主场景视口 */
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

	/* 显示菜单栏UI */
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

				ImGui::EndMenuBar();
			}
		}
		ImGui::End();
	}

	/* 显示主场景视口 */
	void ModelEditorLayer::ShowSceneViewportUI()
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

			///* 拖动资源到主窗口 */
			//if (ImGui::BeginDragDropTarget())
			//{
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
			//	{
			//		const wchar_t* path = (const wchar_t*)payload->Data;
			//		OnDragItemToScene(g_AssetPath / path);
			//	}
			//	ImGui::EndDragDropTarget();
			//}

			///* Gizmos */
			//ShowOperationGizmoUI();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	/* 显示模型编辑UI */
	void ModelEditorLayer::ShowModelParamsUI()
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

						if (ImGui::TreeNode(sub_model_info.Name.empty() ? "Unnamed" : sub_model_info.Name.c_str()))
						{
							/* 基本信息 */
							ImGui::Text("Base Info:");
							{
								const auto& aabb_min = sub_model_info.AABB.first;
								const auto& aabb_max = sub_model_info.AABB.second;
								ImGui::BulletText("AABB Min: (%f, %f, %f)", aabb_min.x, aabb_min.y, aabb_min.z);
								ImGui::BulletText("AABB Max: (%f, %f, %f)", aabb_max.x, aabb_max.y, aabb_max.z);
								ImGui::BulletText("Vertex Cnt: %d", sub_model_info.VertexArray->GetVertexCount());
							}

							/* 材质信息 */
							if (ImGui::TreeNode("Ambient"))
							{
								/* 数据类型 */
								const char* data_types[] = { "Texture", "Float", "Int", "Vec2", "Vec3", "Vec4" };
								static int current_type_idx = 0;
								const char* preview_value = data_types[current_type_idx];
								if (ImGui::BeginCombo("DataType", preview_value))
								{
									for (int n = 0; n < IM_ARRAYSIZE(data_types); n++)
									{
										const bool is_selected = (current_type_idx == n);
										if (ImGui::Selectable(data_types[n], is_selected))
											current_type_idx = n;

										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}

								/* 若数据类型为纹理 */
								if (current_type_idx == 0)
								{
									TextureLoadConfig load_config;
									load_config.IsFlipV = false;
									const auto& show_texture = sub_model_info.MaterialParams.AmbientTexPath.empty() ? TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/default_texture.png", {}) : TextureAssetManager::Instance().GetOrCreateTexture(sub_model_info.MaterialParams.AmbientTexPath, load_config);

									/* Image */
									ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
									if (ImGui::BeginDragDropTarget())
									{
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
										{
											const wchar_t* path = (const wchar_t*)payload->Data;
											const std::filesystem::path texture_path = path;

											material->SetTexture("", TextureAssetManager::Instance().GetOrCreateTexture(texture_path.string(), load_config), TextureSlot::Ambient);
											sub_model_info.MaterialParams.AmbientTexPath = texture_path.string();
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
								}

								ImGui::TreePop();
							}

							if (ImGui::TreeNode("Diffuse"))
							{
								/* 数据类型 */
								const char* data_types[] = { "Texture", "Float", "Int", "Vec2", "Vec3", "Vec4" };
								static int current_type_idx = 0;
								const char* preview_value = data_types[current_type_idx];
								if (ImGui::BeginCombo("DataType", preview_value))
								{
									for (int n = 0; n < IM_ARRAYSIZE(data_types); n++)
									{
										const bool is_selected = (current_type_idx == n);
										if (ImGui::Selectable(data_types[n], is_selected))
											current_type_idx = n;

										// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
										if (is_selected)
											ImGui::SetItemDefaultFocus();
									}
									ImGui::EndCombo();
								}

								/* 若数据类型为纹理 */
								if (current_type_idx == 0)
								{
									TextureLoadConfig load_config;
									load_config.IsFlipV = false;
									const auto& show_texture = sub_model_info.MaterialParams.DiffuseTexPath.empty() ? TextureAssetManager::Instance().GetOrCreateTexture("assets/textures/default_texture.png", {}) : TextureAssetManager::Instance().GetOrCreateTexture(sub_model_info.MaterialParams.DiffuseTexPath, load_config);

									/* Image */
									ImGui::ImageButton((ImTextureID)show_texture->GetTextureID(), ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0), 0);
									if (ImGui::BeginDragDropTarget())
									{
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
										{
											const wchar_t* path = (const wchar_t*)payload->Data;
											const std::filesystem::path texture_path = path;

											material->SetTexture("u_AlbedoTexture", TextureAssetManager::Instance().GetOrCreateTexture(texture_path.string(), load_config), TextureSlot::Albedo);
											sub_model_info.MaterialParams.DiffuseTexPath = texture_path.string();
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
								}

								ImGui::TreePop();
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

	/* 导入模型 */
	void ModelEditorLayer::ImportModel()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("Obj(*.obj)\0*.obj\0");
		if (!file_path.empty())
		{
			/* 读取模型 */
			m_pModelInfo->Reset();
			m_pModelInfo->LoadFromObj(file_path);

			/* 初始化材质 */
			m_pMaterialGroup->ClearAllMaterials();
			for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
				m_pMaterialGroup->EmplaceMaterial(Material::Create(ShaderAssetManager::Instance().GetOrLoad("assets/shaders/default.glsl")));

			/* 更新场景模型信息 */
			UpdateModel();
		}
	}

	/* 导出模型 */
	void ModelEditorLayer::ExportMeshAndMtl()
	{
		if (!m_pModelInfo || !m_pModel)
			return;

		const auto& model_path = m_pModelInfo->m_Path;
		const auto& mesh_path = model_path + ".mesh";
		//m_pModel->

		const auto& material_path = model_path + ".mtl";
		m_pMaterialGroup->Serializer(material_path);
		m_pMaterialGroup->Deserializer(material_path);
	}

	/* 更新模型 */
	void ModelEditorLayer::UpdateModel()
	{
		/* 先移除场景中的其他模型 */
		m_pDefaultScene->DestroyTargetEntities<ModelComponent>();

		/* 新建模型 */
		m_pModel = CreateSharedPtr<Model>(m_pModelInfo->m_Path);
		for (size_t i = 0; i < m_pModelInfo->m_SubModelInfos.size(); ++i)
		{
			auto& sub_model_info = m_pModelInfo->m_SubModelInfos[i];
			auto& material = m_pMaterialGroup->GetMaterialByIndex(i);

			/* 更新材质 */
			UpdateMaterial(material, sub_model_info.MaterialParams);
			
			SharedPtr<MeshSegment> mesh_segment = CreateSharedPtr<MeshSegment>(sub_model_info.Name, sub_model_info.VertexArray, material);
			mesh_segment->SetAABB(sub_model_info.AABB.first, sub_model_info.AABB.second);

			m_pModel->AddMeshSegment(mesh_segment);
		}

		/* 将模型添加到场景中 */
		Entity entity = m_pDefaultScene->CreateEntity(m_pModel->GetDebugName());
		auto& mesh_component = entity.AddComponent<ModelComponent>();
		mesh_component.Model = m_pModel;

		/* todo: 根据模型大小自适应相机距离 */
		const glm::vec3 center = (m_pModel->GetAABBMin() + m_pModel->GetAABBMax()) * 0.5f;
		auto& transform_component = entity.GetComponent<TransformComponent>();
		transform_component.Position = -center;
		transform_component.Scale = glm::vec3(0.5f);
	}

	/* 更新材质：根据材质参数设置材质 */
	void ModelEditorLayer::UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params)
	{
		TextureLoadConfig load_config;
		load_config.IsFlipV = false;
		{
			/* Ambient */
			if (!material_params.AmbientTexPath.empty())
				material->SetTexture("u_AmbientTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.AmbientTexPath, load_config), TextureSlot::Ambient);
			else
				material->SetParameters(ParamType::Vec3, "Ambient", material_params.Ambient);

			/* Diffuse/Albedo */
			if (!material_params.DiffuseTexPath.empty())
				material->SetTexture("u_AlbedoTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.DiffuseTexPath, load_config), TextureSlot::Albedo);
			else
				material->SetParameters(ParamType::Vec3, "Diffuse", material_params.Diffuse);

			/* Specular */
			if (!material_params.SpecularTexPath.empty())
				material->SetTexture("u_SpecularTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.SpecularTexPath, load_config), TextureSlot::Specular);
			else
				material->SetParameters(ParamType::Vec3, "Specular", material_params.Specular);

			/* Normal, todo: 处理Bump和Displacement */
			if (!material_params.BumpTexPath.empty())
				material->SetTexture("u_NormalTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.BumpTexPath, load_config), TextureSlot::Normal);
			if (!material_params.DisplacementTexPath.empty())
				material->SetTexture("u_NormalTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.DisplacementTexPath, load_config), TextureSlot::Normal);

			/* Roughness */
			if (!material_params.RoughnessTexPath.empty())
				material->SetTexture("u_RoughnessTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.RoughnessTexPath, load_config), TextureSlot::Roughness);
			else
				material->SetParameters(ParamType::Float, "Roughness", material_params.Roughness);

			/* Metallic */
			if (!material_params.MetallicTexPath.empty())
				material->SetTexture("u_MetallicTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.MetallicTexPath, load_config), TextureSlot::Metallic);
			else
				material->SetParameters(ParamType::Float, "Metallic", material_params.Metallic);

			/* Emission */
			if (!material_params.EmissionTexPath.empty())
				material->SetTexture("u_EmissiveTexture", TextureAssetManager::Instance().GetOrCreateTexture(material_params.EmissionTexPath, load_config), TextureSlot::Emissive);
			else
				material->SetParameters(ParamType::Vec3, "Emission", material_params.Emission);

			/* ClearCoat */
			material->SetParameters(ParamType::Float, "ClearCoatRoughness", material_params.ClearCoatRoughness);
			material->SetParameters(ParamType::Float, "ClearCoatThickness", material_params.ClearCoatThickness);

			/* Others */
			material->SetParameters(ParamType::Vec3, "Transmittance", material_params.Transmittance);
			material->SetParameters(ParamType::Float, "IOR", material_params.IOR);
		}
	}
}
