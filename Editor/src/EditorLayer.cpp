#include "pch.h"
#include "EditorLayer.h"
#include <glm/gtc/type_ptr.inl>
#include "EditorAssetManager.h"
#include "EditorUIFunctions.h"
#include "ImGuizmo.h"
#include "Wuya/Events/KeyEvent.h"
#include "Wuya/Events/MouseEvent.h"

extern const std::filesystem::path g_AssetPath;

EditorLayer::EditorLayer()
	: ILayer("EditorLayer")
{
	PROFILE_FUNCTION();
}

void EditorLayer::OnAttached()
{
	PROFILE_FUNCTION();

	// Texture
	m_pTexture2D = Wuya::Texture2D::Create("assets/textures/game-sky.jpg");

	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/triangle.glsl");
	shader->Bind();
	shader->SetFloat3("color", glm::vec3(1, 1, 0));
	shader->SetInt("u_Texture", 0);

	Wuya::Renderer::Init();

	m_pVertexArray = Wuya::VertexArray::Create();
	m_pVertexArray->Bind();

	// triangle vertices
	const float vertices[3 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	Wuya::SharedPtr<Wuya::VertexBuffer> vertex_buffer = Wuya::VertexBuffer::Create(vertices, sizeof(vertices));
	Wuya::VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", Wuya::BufferDataType::Float3 }
	};
	vertex_buffer->SetLayout(vertex_buffer_layout);
	m_pVertexArray->AddVertexBuffer(vertex_buffer);

	// Indices
	const uint32_t indices[3] = {
		0, 1, 2
	};
	Wuya::SharedPtr<Wuya::IndexBuffer> index_buffer = Wuya::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_pVertexArray->SetIndexBuffer(index_buffer);

	// Camera
	m_pEditorCamera = Wuya::CreateSharedPtr<Wuya::EditorCamera>(30.0f);
	m_pOrthographicCameraController = Wuya::CreateSharedPtr<Wuya::OrthographicCameraController>(m_pEditorCamera->GetAspectRatio());
	//m_pEditorCamera->SetViewportSize(1280, 720);
	//m_pEditorCamera->SetDistance(1);

	// Frame buffer
	Wuya::FrameBufferDescription desc;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Attachments = { Wuya::FrameBufferTargetFormat::RGBA8, Wuya::FrameBufferTargetFormat::RedInteger, Wuya::FrameBufferTargetFormat::Depth24Stencil8 };
	m_pFrameBuffer = Wuya::FrameBuffer::Create(desc);

	m_pMainScene = Wuya::CreateSharedPtr<Wuya::Scene>();
	m_SceneHierarchy.SetOwnerScene(m_pMainScene);
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
	/* 主窗口 */
	ShowSceneViewportUI();
	/* 场景控制UI */
	ShowSceneControllerUI();
	/* 统计信息 */
	ShowStatisticInfoUI();
	/* 场景管理及属性窗口 */
	m_SceneHierarchy.OnImGuiRender();
	/* 资源管理窗口 */
	m_ResourceBrowser.OnImGuiRenderer();

	bool open = true;
	ImGui::ShowDemoWindow(&open);
}

void EditorLayer::OnEvent(Wuya::IEvent* event)
{
	PROFILE_FUNCTION();

	m_pOrthographicCameraController->OnEvent(event);
	m_pEditorCamera->OnEvent(event);

	Wuya::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Wuya::KeyPressedEvent>(BIND_EVENT_FUNC(EditorLayer::OnKeyPressed));
	dispatcher.Dispatch<Wuya::MouseButtonPressedEvent>(BIND_EVENT_FUNC(EditorLayer::OnMouseButtonPressed));
}

void EditorLayer::UpdateViewport()
{
	PROFILE_FUNCTION();

	const Wuya::FrameBufferDescription desc = m_pFrameBuffer->GetDescription();
	if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 &&
		(desc.Width != m_ViewportSize.x || desc.Height != m_ViewportSize.y))
	{
		m_pFrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_pEditorCamera->SetViewportSize(static_cast<float>(m_ViewportSize.x), static_cast<float>(m_ViewportSize.y));
		m_pOrthographicCameraController->OnResize(static_cast<float>(m_ViewportSize.x), static_cast<float>(m_ViewportSize.y));
	}
}

bool EditorLayer::OnKeyPressed(Wuya::KeyPressedEvent* event)
{
	PROFILE_FUNCTION();

	if (event->GetRepeatCount() > 0)
		return false;

	bool is_ctrl_pressed = Wuya::Input::IsKeyPressed(Wuya::Key::LeftControl) || Wuya::Input::IsKeyPressed(Wuya::Key::RightControl);

	switch (event->GetKeyCode())
	{
	case Wuya::Key::N: /* Ctrl+N：新建一个场景 */
	{
		if (is_ctrl_pressed)
			NewScene();
		break;
	}
	case Wuya::Key::S: /* Ctrl+S: 保存场景 */
	{
		if (is_ctrl_pressed)
			SaveScene();
		break;
	}
	case Wuya::Key::Q:
	{
		if (!ImGuizmo::IsUsing())
			m_GizmoType = -1;
		break;
	}
	case Wuya::Key::W:
	{
		if (!ImGuizmo::IsUsing())
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		break;
	}
	case Wuya::Key::E:
	{
		if (!ImGuizmo::IsUsing())
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		break;
	}
	case Wuya::Key::R:
	{
		if (!ImGuizmo::IsUsing())
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
		break;
	}
	}

	return true;
}

bool EditorLayer::OnMouseButtonPressed(Wuya::MouseButtonPressedEvent* event)
{
	PROFILE_FUNCTION();

	if (event->GetMouseButton() == Wuya::Mouse::ButtonLeft)
	{
		if (m_IsViewportHovered && !ImGuizmo::IsOver() && !Wuya::Input::IsKeyPressed(Wuya::Key::LeftAlt))
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

	m_pMainScene = Wuya::CreateSharedPtr<Wuya::Scene>();
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

	m_pMainScene->Serializer("assets/scenes/test.scn");
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
	PROFILE_FUNCTION();

	if (path.extension().string() != ".scn")
	{
		EDITOR_LOG_ERROR("File is not a scene file: {}.", path.filename().string());
		return;
	}

	Wuya::SharedPtr<Wuya::Scene> new_scene = Wuya::CreateSharedPtr<Wuya::Scene>();
	if (new_scene->Deserializer(path.string()))
	{
		m_pMainScene = new_scene;
		m_SceneHierarchy.SetOwnerScene(m_pMainScene);
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

				if(ImGui::MenuItem("Open Scene", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Exit"))
					Wuya::Application::Instance()->Close();

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

	static Wuya::SharedPtr<Wuya::Texture2D> save_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/save.png");
	static Wuya::SharedPtr<Wuya::Texture2D> translate_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/translate.png");
	static Wuya::SharedPtr<Wuya::Texture2D> rotate_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/rotate.png");
	static Wuya::SharedPtr<Wuya::Texture2D> scale_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/scale.png");
	static Wuya::SharedPtr<Wuya::Texture2D> play_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/play.png");
	static Wuya::SharedPtr<Wuya::Texture2D> stop_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/stop.png");

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

		/* 切换执行模式 */
		ImGui::SameLine();
		const Wuya::SharedPtr<Wuya::Texture2D> icon = (m_PlayMode == PlayMode::Edit) ? play_icon : stop_icon;
		ImGui::SetCursorPosX((panel_width - icon_size) * 0.5f);
		if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
		{
			OnPlayModeChanged();
		}

		END_TRANSPARENT_BUTTON;
	}

	ImGui::End();
	ImGui::PopStyleVar(2);
}

void EditorLayer::ShowSceneViewportUI()
{
	PROFILE_FUNCTION();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Scene");
	{
		/* 获取窗口范围 */
		const auto viewport_region_min = ImGui::GetWindowContentRegionMin();
		const auto viewport_region_max = ImGui::GetWindowContentRegionMax();
		const auto viewport_offset = ImGui::GetWindowPos();
		m_ViewportRegion = {
			viewport_region_min.x + viewport_offset.x,
			viewport_region_min.y + viewport_offset.y,
			viewport_region_max.x + viewport_offset.x,
			viewport_region_max.y + viewport_offset.y
		};

		/* 若当前ImGui窗口不是主窗口，应阻塞事件传递 */
		m_IsViewportFocused = ImGui::IsWindowFocused();
		m_IsViewportHovered = ImGui::IsWindowHovered();
		Wuya::Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

		auto viewport_panel_size = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewport_panel_size.x, viewport_panel_size.y };

		/* 绘制场景 */
		const uint64_t texture_id = m_pFrameBuffer->GetColorAttachmentByIndex(0);
		ImGui::Image((ImTextureID)texture_id, viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

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
			auto statistics = Wuya::Renderer2D::GetStatisticsInfo();
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
	// Editor camera
	const glm::mat4& projection_mat = m_pEditorCamera->GetProjectionMatrix();
	glm::mat4 view_mat = m_pEditorCamera->GetViewMatrix();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(m_ViewportRegion.x, m_ViewportRegion.y, m_ViewportRegion.z - m_ViewportRegion.x, m_ViewportRegion.w - m_ViewportRegion.y);

	Wuya::Entity selected_entity = m_SceneHierarchy.GetSelectedEntity();
	if (selected_entity && m_GizmoType != -1)
	{
		// Entity transform
		auto& transform_component = selected_entity.GetComponent<Wuya::TransformComponent>();
		glm::mat4 transform_mat = transform_component.GetTransform();

		// Snapping
		bool snap = Wuya::Input::IsKeyPressed(Wuya::Key::LeftControl);
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

	/* 右上角方位视图 */
	ImGuizmo::ViewManipulate(glm::value_ptr(view_mat), m_pEditorCamera->GetDistance(), ImVec2(m_ViewportRegion.z - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x00000000);
	m_pEditorCamera->SetViewMatrix(view_mat);

}

void EditorLayer::OnUpdate(float delta_time)
{
	PROFILE_FUNCTION();

	UpdateViewport();

	m_pFrameBuffer->Bind();

	Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();
	m_pFrameBuffer->ClearColorAttachment(1, -1);

	if (m_pMainScene)
	{
		switch (m_PlayMode)
		{
			case PlayMode::Edit:
				/* 更新相机信息 */
				if (m_IsViewportFocused)
				{
					m_pOrthographicCameraController->OnUpdate(delta_time);
					m_pEditorCamera->OnUpdate();
				}

				/* 更新场景中的实体 */
				m_pMainScene->OnUpdateEditor(m_pEditorCamera, delta_time);
				break;
		case PlayMode::Runtime: 
			break;
		default: 
			ASSERT(false);
			break;
		}
	}

	//m_pTexture2D->Bind();

	//auto shader = m_pShaderLibrary->GetShaderByName("triangle");
	///*glm::mat4 scale = glm::mat4(
	//	2, 0, 0, 0,
	//	0, 2, 0, 0,
	//	0, 0, 2, 0,
	//	0, 0, 0, 1);
	//shader->SetMat4("u_ViewProjectionMat", scale);*/
	//shader->SetMat4("u_ViewProjectionMat", m_pEditorCamera->GetViewProjectionMatrix());
	//Wuya::Renderer::Submit(shader, m_pVertexArray);


	// todo: 鼠标点击选中物体
	auto [mouse_x, mouse_y] = ImGui::GetMousePos();
	mouse_x -= m_ViewportRegion.x;
	mouse_y -= m_ViewportRegion.y;

	const glm::vec2 viewport_size = glm::vec2(m_ViewportRegion.z - m_ViewportRegion.x, m_ViewportRegion.w - m_ViewportRegion.y);
	mouse_y = viewport_size.y - mouse_y;

	if (mouse_x > 0 && mouse_y > 0 && mouse_x < viewport_size.x && mouse_y < viewport_size.y)
	{
		int pixel_data = m_pFrameBuffer->ReadPixel(1, (int)mouse_x, (int)mouse_y);
		m_HoveredEntity = pixel_data == -1 ? Wuya::Entity() : Wuya::Entity((entt::entity)pixel_data, m_pMainScene.get());
		//EDITOR_LOG_ERROR(pixel_data);
	}

	m_pFrameBuffer->Unbind();
}
