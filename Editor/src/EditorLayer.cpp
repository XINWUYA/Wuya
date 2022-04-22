#include "EditorLayer.h"
#include <imgui.h>

#include "EditorAssetManager.h"
#include "EditorUICreator.h"
#include "Wuya/Events/KeyEvent.h"
#include "Wuya/Events/MouseEvent.h"

EditorLayer::EditorLayer()
	: ILayer("EditorLayer")
{
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

void EditorLayer::NewScene()
{
	m_pMainScene = Wuya::CreateSharedPtr<Wuya::Scene>();
	m_SceneHierarchy.SetOwnerScene(m_pMainScene);
}

void EditorLayer::OpenScene()
{
	//todo: 打开一个序列化的场景
}

void EditorLayer::SaveScene()
{
}

void EditorLayer::UpdateViewport()
{
	const auto desc = m_pFrameBuffer->GetDescription();
	if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 &&
		(desc.Width != m_ViewportSize.x || desc.Height != m_ViewportSize.y))
	{
		m_pFrameBuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_pEditorCamera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_pOrthographicCameraController->OnResize(m_ViewportSize.x, m_ViewportSize.y);
	}
}

bool EditorLayer::OnKeyPressed(Wuya::KeyPressedEvent* event)
{
	if (event->GetRepeatCount() > 0)
		return false;

	bool is_ctrl_pressed = Wuya::Input::IsKeyPressed(Wuya::Key::LeftControl) || Wuya::Input::IsKeyPressed(Wuya::Key::RightControl);

	switch (event->GetKeyCode())
	{
	case Wuya::Key::N: // Ctrl+N：新建一个场景
	{
		if (is_ctrl_pressed)
			NewScene();
		break;
	}
	}
}

bool EditorLayer::OnMouseButtonPressed(Wuya::MouseButtonPressedEvent* event)
{
	if (event->GetMouseButton() == Wuya::Mouse::ButtonLeft)
	{
		if (m_IsViewportHovered)
			m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
	}
	return false;
}

void EditorLayer::OnPlayModeChanged()
{
	if (m_PlayMode == PlayMode::Edit) /* 编辑模式切换为运行模式 */
	{
		m_PlayMode = PlayMode::Runtime;
	}
	else if (m_PlayMode == PlayMode::Runtime) /* 运行模式切换为编辑模式 */
	{
		m_PlayMode = PlayMode::Edit;
	}

}

void EditorLayer::ShowMenuUI()
{
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
	static Wuya::SharedPtr<Wuya::Texture2D> play_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/play.png");
	static Wuya::SharedPtr<Wuya::Texture2D> stop_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/stop.png");

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2)); /* 指定间隔 */
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 2));

	ImGui::Begin("##Scene Controller", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		const float icon_size = ImGui::GetWindowHeight() - 4.0f;
		const float panel_width = ImGui::GetWindowContentRegionMax().x;
		START_TRANSPARENT_BUTTON;
		const auto icon = (m_PlayMode == PlayMode::Edit) ? play_icon : stop_icon;
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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Scene");
	{
		// 若当前ImGui窗口不是主窗口，应阻塞事件传递
		m_IsViewportFocused = ImGui::IsWindowFocused();
		m_IsViewportHovered = ImGui::IsWindowHovered();
		Wuya::Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

		auto viewport_panel_size = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewport_panel_size.x, viewport_panel_size.y };

		// 绘制场景
		const uint64_t texture_id = m_pFrameBuffer->GetColorAttachmentByIndex(0);
		ImGui::Image((ImTextureID)(texture_id), viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// 拖动资源到主窗口
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
			{
				// todo
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorLayer::ShowStatisticInfoUI()
{
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

	m_pFrameBuffer->Unbind();

	// todo: 鼠标点击选中物体
}
