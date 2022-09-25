#include "Pch.h"
#include "EditorLayer.h"
#include <glm/gtc/type_ptr.inl>
#include "EditorAssetManager.h"
#include "EditorBuiltinCamera.h"
#include "EditorUIFunctions.h"
#include "ImGuizmo.h"
#include "Wuya/Renderer/RenderView.h"
#include "Wuya/Renderer/FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/Model.h"

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: ILayer("EditorLayer")
	{
		PROFILE_FUNCTION();
	}

	void EditorLayer::OnAttached()
	{
		PROFILE_FUNCTION();
		
		Renderer::Init();

		// Camera
		m_pEditorCamera = CreateSharedPtr<EditorCamera>(30.0f);

		m_pMainScene = CreateSharedPtr<Scene>();
		m_SceneHierarchy.SetOwnerScene(m_pMainScene);

		//{
		//	// Material
		//	auto material = CreateSharedPtr<Material>();
		//	const auto albedo_texture = Texture::Create("assets/textures/container.jpg");
		//	material->SetTexture(albedo_texture, 0);
		//	const auto shader = ShaderLibrary::Instance().GetOrLoad("assets/shaders/simple.glsl");
		//	material->SetShader(shader);

		//	Entity entity = m_pMainScene->CreateEntity("Cube");
		//	auto& mesh_component = entity.AddComponent<ModelComponent>();
		//	mesh_component.Model = Model::Create(BuiltinModelType::Cube, material);
		//	auto& transform_component = entity.GetComponent<TransformComponent>();
		//	transform_component.Position = glm::vec3(3, 0, 0);
		//}

		/*{
			Entity entity = m_pMainScene->CreateEntity("Dragon");
			auto& mesh_component = entity.AddComponent<ModelComponent>();
			mesh_component.Model = Model::Create("assets/models/dragon/dragon.obj");
			auto& transform_component = entity.GetComponent<TransformComponent>();
			transform_component.Position = glm::vec3(0, 0, 0);
			transform_component.Scale = glm::vec3(5, 5, 5);
		}

		{
			Entity entity = m_pMainScene->CreateEntity("Nanosuit");
			auto& mesh_component = entity.AddComponent<ModelComponent>();
			mesh_component.Model = Model::Create("assets/models/nanosuit/nanosuit.obj");
			auto& transform_component = entity.GetComponent<TransformComponent>();
			transform_component.Position = glm::vec3(-3, -1, 0);
			transform_component.Scale = glm::vec3(0.2, 0.2, 0.2);
		}*/
	}

	void EditorLayer::OnDetached()
	{
		PROFILE_FUNCTION();

		ILayer::OnDetached();
	}

	void EditorLayer::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* 菜单 */
		ShowMenuUI();
		/* 场景控制UI */
		ShowSceneControllerUI();
		/* 统计信息 */
		ShowStatisticInfoUI();
		/* 场景管理及属性窗口 */
		m_SceneHierarchy.OnImGuiRender();
		/* 资源管理窗口 */
		m_ResourceBrowser.OnImGuiRenderer();

		/* 主窗口，需要最后再画，以确保能够得到正确的窗口宽高 */
		ShowSceneViewportUI();

		//bool open = true;
		//ImGui::ShowDemoWindow(&open);
	}

	void EditorLayer::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		if (!event)
			return;

		//m_pOrthographicCameraController->OnEvent(event);
		m_pEditorCamera->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNC(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::UpdateViewport()
	{
		PROFILE_FUNCTION();

		//const FrameBufferDescription desc = m_pFrameBuffer->GetDescription();
		if (m_ViewportRegion.Width > 0 && m_ViewportRegion.Height > 0/* && (desc.Width != m_ViewportRegion.Width() || desc.Height != m_ViewportRegion.Height())*/)
		{
			//m_pFrameBuffer->Resize(m_ViewportRegion.Width(), m_ViewportRegion.Height());
			m_pEditorCamera->SetViewportRegion({ 0,0,m_ViewportRegion.Width, m_ViewportRegion.Height });
			//m_pOrthographicCameraController->OnResize(static_cast<float>(m_ViewportRegion.Width()), static_cast<float>(m_ViewportRegion.Height()));
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent* event)
	{
		PROFILE_FUNCTION();

		if (event->GetRepeatCount() > 0)
			return false;

		bool is_ctrl_pressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool is_button_right_pressed = Input::IsMouseButtonPressed(Mouse::ButtonRight);

		switch (event->GetKeyCode())
		{
		case Key::N: /* Ctrl+N：新建一个场景 */
		{
			if (is_ctrl_pressed)
				NewScene();
			break;
		}
		case Key::S: /* Ctrl+S: 保存场景 */
		{
			if (is_ctrl_pressed)
				SaveScene();
			break;
		}
		case Key::Q:
		{
			if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				m_GizmoType = -1;
			break;
		}
		case Key::W:
		{
			if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}
		case Key::E:
		{
			if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
		case Key::R:
		{
			if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			break;
		}
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent* event)
	{
		PROFILE_FUNCTION();

		if (event->GetMouseButton() == Mouse::ButtonLeft)
		{
			CheckMouseSelectEntity();

			if (m_IsViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnPlayModeChanged()
	{
		PROFILE_FUNCTION();

		if (m_PlayMode == PlayMode::Edit) /* 编辑模式切换为运行模式 */
		{
			m_PlayMode = PlayMode::Runtime;
		}
		else if (m_PlayMode == PlayMode::Runtime) /* 运行模式切换为编辑模式 */
		{
			m_PlayMode = PlayMode::Edit;
		}

	}

	void EditorLayer::NewScene()
	{
		PROFILE_FUNCTION();

		m_pMainScene = CreateSharedPtr<Scene>();
		m_SceneHierarchy.SetOwnerScene(m_pMainScene);
	}

	void EditorLayer::OpenScene()
	{
		PROFILE_FUNCTION();

		//std::string file_path = FileDialog
		//todo: 打开一个序列化的场景
	}

	void EditorLayer::SaveScene()
	{
		PROFILE_FUNCTION();

		m_pMainScene->Serializer("assets/scenes/test1.scn");
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		if (path.extension().string() != ".scn")
		{
			EDITOR_LOG_ERROR("File is not a scene file: {}.", path.filename().string());
			return;
		}

		SharedPtr<Scene> new_scene = CreateSharedPtr<Scene>();
		if (new_scene->Deserializer(path.string()))
		{
			m_pMainScene = new_scene;
			m_SceneHierarchy.SetOwnerScene(m_pMainScene);

			/* 切换场景时，需更新Editor RenderView的场景 */
			m_pEditorCamera->GetRenderView()->SetOwnerScene(m_pMainScene);
		}
	}

	void EditorLayer::ShowMenuUI()
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
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
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
					if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					{
						NewScene();
					}

					if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
					{
						OpenScene();
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

	void EditorLayer::ShowSceneControllerUI()
	{
		PROFILE_FUNCTION();

		static auto save_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/save.png");
		static auto translate_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/translate.png");
		static auto rotate_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/rotate.png");
		static auto scale_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/scale.png");
		static auto play_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/play.png");
		static auto stop_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/stop.png");
		static auto menu_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/menu.png");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2)); /* 指定间隔 */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 2));

		ImGui::Begin("##Scene Controller", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			const float icon_size = ImGui::GetWindowHeight() - 4.0f;
			const float panel_width = ImGui::GetWindowContentRegionMax().x;

			START_TRANSPARENT_BUTTON;

			constexpr float cursor_offset = 10.0f;
			/* 保存场景按钮 */
			ImGui::SetCursorPosX(cursor_offset);
			if (ImGui::ImageButton((ImTextureID)save_icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
			{
				/*ImGui::OpenPopup("Save Scene");
				EditorUIFunctions::RegisterPopupFunc("Save Scene", [&]()
				{
					bool never;
					EditorUIFunctions::DrawModalUI("Save Scene", "Whether to save the current scene", never);
				});*/
				SaveScene();
			}
			//EditorUIFunctions::DrawPopups();

			/* 移动/旋转/平移操作 */
			{
				/* translate */
				ImGui::SameLine(cursor_offset + icon_size * 2);
				bool checked = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Translate", translate_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					});

				/* rotate */
				ImGui::SameLine();
				checked = m_GizmoType == ImGuizmo::OPERATION::ROTATE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Rotate", rotate_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					});

				/* scale */

				ImGui::SameLine();
				checked = m_GizmoType == ImGuizmo::OPERATION::SCALE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Scale", scale_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
					});
			}

			/* 切换执行模式 */
			{
				ImGui::SameLine();
				const SharedPtr<Texture> icon = (m_PlayMode == PlayMode::Edit) ? play_icon : stop_icon;
				ImGui::SetCursorPosX((panel_width - icon_size) * 0.5f);
				if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
				{
					OnPlayModeChanged();
				}
			}

			/* 配置 */
			{
				ImGui::SameLine(panel_width - cursor_offset - 20);
				START_STYLE_ALPHA(0.5f);
				if (ImGui::ImageButton((ImTextureID)menu_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					ImGui::OpenPopup("ConfigPopup");
				END_STYLE_ALPHA;

				/* 展开弹窗时，显示控件 */
				if (ImGui::BeginPopup("ConfigPopup"))
				{
					ImGui::PushItemWidth(200);

					bool is_focus = m_pEditorCamera->IsFocus();
					ImGui::Checkbox("FocusMode", &is_focus);
					m_pEditorCamera->SetFocus(is_focus);

					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
			}

			END_TRANSPARENT_BUTTON;
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::ShowSceneViewportUI()
	{
		PROFILE_FUNCTION();

		//m_ViewportRegion = { 268, 2188, 317, 1371 };

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

			/* 若当前ImGui窗口不是主窗口，应阻塞事件传递 */
			m_IsViewportFocused = ImGui::IsWindowFocused();
			m_IsViewportHovered = ImGui::IsWindowHovered();
			Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

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
					OpenScene(g_AssetPath / path);
				}
				ImGui::EndDragDropTarget();
			}

			/* Gizmos */
			ShowOperationGizmoUI();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::ShowStatisticInfoUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Stat Info");
		{
			// todo: 帧率等
			// Renderer2D Stats
			if (ImGui::CollapsingHeader("2D"))
			{
				auto statistics = Renderer2D::GetStatisticsInfo();
				ImGui::BulletText("Draw Calls: %d", statistics.DrawCalls);
				ImGui::BulletText("Quads: %d", statistics.QuadCount);
				ImGui::BulletText("Vertices: %d", statistics.TotalVertexCount());
				ImGui::BulletText("Indices: %d", statistics.TotalIndexCount());
			}
			// Renderer3D Stats
			if (ImGui::CollapsingHeader("3D"))
			{
				// todo: 三角形、模型数量
			}
		}
		ImGui::End();
	}

	void EditorLayer::ShowOperationGizmoUI()
	{
		PROFILE_FUNCTION();

		// Editor camera
		const glm::mat4& projection_mat = m_pEditorCamera->GetProjectionMatrix();
		glm::mat4 view_mat = m_pEditorCamera->GetViewMatrix();

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_ViewportRegion.MinX, m_ViewportRegion.MinY, m_ViewportRegion.Width, m_ViewportRegion.Height);

		Entity selected_entity = m_SceneHierarchy.GetSelectedEntity();
		if (selected_entity && m_GizmoType != -1)
		{
			// Entity transform
			auto& transform_component = selected_entity.GetComponent<TransformComponent>();
			glm::mat4 transform_mat = transform_component.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snap_value = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snap_value = 45.0f;

			float snap_values[3] = { snap_value, snap_value, snap_value };

			ImGuizmo::Manipulate(glm::value_ptr(view_mat), glm::value_ptr(projection_mat),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform_mat),
				nullptr, snap ? snap_values : nullptr); /* ImGuizmo::LOCAL/ ImGuizmo::WORLD */

			if (ImGuizmo::IsUsing())
			{
				/* 从变换矩阵中恢复 */
				glm::vec3 position, rotation, scale;
				DecomposeTransform(transform_mat, position, rotation, scale);

				/* 更新组件信息 */
				glm::vec3 delta_rotation = rotation - transform_component.Rotation;
				transform_component.Position = position;
				transform_component.Rotation += delta_rotation;
				transform_component.Scale = scale;
			}
		}

		/* 坐标线框
		 *
		 * todo: 将线框划到场景物体之后
		 */
		//const auto identity_mat = glm::identity<glm::mat4>();
		//ImGuizmo::DrawGrid(glm::value_ptr(view_mat), glm::value_ptr(projection_mat), glm::value_ptr(identity_mat), 100.f);

		/* 右上角方位视图 */
		/*if (!m_pEditorCamera->IsFocus())
		{
			ImGuizmo::ViewManipulate(glm::value_ptr(view_mat), m_pEditorCamera->GetDistance(), ImVec2(m_ViewportRegion.z - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x00000000);
			m_pEditorCamera->SetViewMatrix(view_mat);
		}*/

	}

	void EditorLayer::CheckMouseSelectEntity()
	{
		PROFILE_FUNCTION();

		// todo: 存在bug
		auto [mouse_x, mouse_y] = ImGui::GetMousePos();
		mouse_x -= m_ViewportRegion.MinX;
		mouse_y -= m_ViewportRegion.MinY;

		const glm::vec2 viewport_size = glm::vec2(m_ViewportRegion.Width, m_ViewportRegion.Height);
		mouse_y = viewport_size.y - mouse_y;

		if (mouse_x > 0 && mouse_y > 0 && mouse_x < viewport_size.x && mouse_y < viewport_size.y)
		{
			int pixel_data = m_pEditorCamera->PickingEntityByPixelPos((int)mouse_x, (int)mouse_y);
			m_HoveredEntity = pixel_data == -1 ? Entity() : Entity((entt::entity)pixel_data, m_pMainScene);
			EDITOR_LOG_ERROR(pixel_data);
		}
	}

	void EditorLayer::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();

		UpdateViewport();

		if (m_ViewportRegion.Width <= 0 || m_ViewportRegion.Height <= 0)
			return;

		Renderer::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Renderer::Clear();
		Renderer::Update();

		auto& editor_render_view = m_pEditorCamera->GetRenderView();
		editor_render_view->SetOwnerScene(m_pMainScene);
		m_pEditorCamera->ConstructRenderView();
		/*auto& editor_frame_graph = editor_render_view->GetFrameGraph();
		struct ImGuiPassData
		{
			FrameGraphResourceHandleTyped<FrameGraphTexture> ColorResult;
		};
		editor_frame_graph->AddPass<ImGuiPassData>("ImGuiPass",
			[&](FrameGraphBuilder& builder, ImGuiPassData& data)
			{
				data.ColorResult = editor_frame_graph->GetBlackboard().GetResourceHandle<FrameGraphTexture>("SidePassOutput");
				builder.BindInputResource(data.ColorResult, FrameGraphTexture::Usage::Sampleable);
				builder.AsSideEffect();
			},
			[&](const FrameGraphResources& resources, const ImGuiPassData& data)
			{
				Renderer::GetRenderAPI()->PushDebugGroup("ImGuiPass");

				auto& texture = resources.Get(data.ColorResult).Texture;
				ImGui::Begin("Scene");
				{
					const auto viewport_panel_size = ImGui::GetContentRegionAvail();
					ImGui::Image((ImTextureID)texture->GetTextureID(), viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				}
				ImGui::End();

				Renderer::GetRenderAPI()->PopDebugGroup();
			}
		);*/


		if (m_pMainScene)
		{
			switch (m_PlayMode)
			{
			case PlayMode::Edit:
				/* 更新相机信息 */
				if (m_IsViewportFocused)
				{
					m_pEditorCamera->OnUpdate(delta_time);
				}

				/* 更新场景中的实体 */
				m_pMainScene->OnUpdateEditor(m_pEditorCamera, delta_time);
				break;
			case PlayMode::Runtime:
				/* 更新场景 */
				m_pMainScene->OnUpdateRuntime(delta_time);
				break;
			default:
				ASSERT(false);
				break;
			}
		}
	}
}
