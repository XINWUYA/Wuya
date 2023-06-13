#include "Pch.h"
#include "SceneEditorLayer.h"
#include <glm/gtc/type_ptr.inl>
#include "EditorBuiltinCamera.h"
#include "EditorUIFunctions.h"
#include "ImGuizmo.h"

namespace Wuya
{
	SceneEditorLayer::SceneEditorLayer()
		: ILayer("SceneEditorLayer")
	{
		PROFILE_FUNCTION();
	}

	void SceneEditorLayer::OnAttached()
	{
		PROFILE_FUNCTION();
		
		// Camera
		m_pEditorCamera = CreateUniquePtr<EditorCamera>("EditorBuiltinCamera", 30.0f);

		m_pMainScene = CreateSharedPtr<Scene>();
		m_SceneHierarchy.SetOwnerScene(m_pMainScene);

		//{
		//	// Material
		//	auto material = CreateSharedPtr<Material>();
		//	const auto albedo_texture = Texture::Create("assets/textures/container.jpg");
		//	material->SetTexture(albedo_texture, 0);
		//	const auto shader = ShaderAssetManager::Instance().GetOrLoad("assets/shaders/simple.glsl");
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

	void SceneEditorLayer::OnDetached()
	{
		PROFILE_FUNCTION();

		ILayer::OnDetached();
	}

	void SceneEditorLayer::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		if (!m_IsActivated)
			return;
		
		/* ͳ����Ϣ */
		ShowStatisticInfoUI();
		/* �����������Դ��� */
		m_SceneHierarchy.OnImGuiRender();

		/* �����ڣ���Ҫ����ٻ�����ȷ���ܹ��õ���ȷ�Ĵ��ڿ�� */
		ShowSceneViewportUI();
	}

	void SceneEditorLayer::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		if (!m_IsActivated || !event)
			return;

		m_pEditorCamera->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(SceneEditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNC(SceneEditorLayer::OnMouseButtonPressed));
	}

	void SceneEditorLayer::UpdateViewport()
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

	bool SceneEditorLayer::OnKeyPressed(KeyPressedEvent* event)
	{
		PROFILE_FUNCTION();

		if (event->GetRepeatCount() > 0)
			return false;

		bool is_ctrl_pressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool is_shift_pressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool is_button_right_pressed = Input::IsMouseButtonPressed(Mouse::ButtonRight);

		switch (event->GetKeyCode())
		{
		case Key::N: /* Ctrl+N���½�һ������ */
			{
				if (is_ctrl_pressed)
					NewScene();
			}
			return true;
		case Key::S: /* Ctrl+S: ���泡��; Ctrl+Shift+S: �������Ϊ */
			if (is_ctrl_pressed)
			{
				if (is_shift_pressed)
					SaveSceneAs();
				else
					SaveScene();
			}
			return true;
		case Key::I: /* Ctrl+I������һ������ */
			{
				if (is_ctrl_pressed)
					ImportScene();
			}
			return true;;
		}

		return false;
	}

	bool SceneEditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent* event)
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

	/* ��Ӧ��ק�ļ��������� */
	void SceneEditorLayer::OnDragItemToScene(const std::filesystem::path& path)
	{
		PROFILE_FUNCTION();

		const auto extension = path.extension().string();

		/* �����ļ� */
		if (extension == ".scn")
		{
			EDITOR_LOG_DEBUG("Import scene file: {}.", path.generic_string());

			SharedPtr<Scene> new_scene = CreateSharedPtr<Scene>();
			if (new_scene->Deserializer(path.generic_string()))
			{
				m_pMainScene = new_scene;
				m_ActiveScenePath = path.generic_string();

				/* ���ö����νṹ����Ӧ�ĳ��� */
				m_SceneHierarchy.SetOwnerScene(m_pMainScene);

				/* �л�����ʱ�������Editor RenderView�ĳ��� */
				m_pEditorCamera->GetRenderView()->SetOwnerScene(m_pMainScene);
			}

			return;
		}

		/* ģ���ļ� */
		if (extension == ".mesh")
		{
			EDITOR_LOG_DEBUG("Import model file: {}.", path.generic_string());

			Entity entity = m_pMainScene->CreateEntity(path.stem().string());
			auto& model_component = entity.AddComponent<ModelComponent>();
			model_component.Model = Model::Create(path.generic_string());

			return;
		}
		
	}

	void SceneEditorLayer::NewScene()
	{
		PROFILE_FUNCTION();

		m_pMainScene = CreateSharedPtr<Scene>();
		m_SceneHierarchy.SetOwnerScene(m_pMainScene);
	}

	void SceneEditorLayer::ImportScene()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("scene(*.scn)\0*.scn\0");
		if (!file_path.empty())
		{
			m_pMainScene->Deserializer(file_path);
			m_ActiveScenePath = file_path;

			/* ���ö����νṹ����Ӧ�ĳ��� */
			m_SceneHierarchy.SetOwnerScene(m_pMainScene);

			/* �л�����ʱ�������Editor RenderView�ĳ��� */
			m_pEditorCamera->GetRenderView()->SetOwnerScene(m_pMainScene);
		}
	}

	void SceneEditorLayer::SaveScene()
	{
		PROFILE_FUNCTION();

		/* ����ǰ������δ���������Ҫ��ȷ��һ������·�� */
		if (m_ActiveScenePath.empty())
			m_ActiveScenePath = FileDialog::SaveFile("scene(*.scn)\0*.scn\0");

		m_pMainScene->Serializer(m_ActiveScenePath);
	}

	/* ���泡����ָ��·�� */
	void SceneEditorLayer::SaveSceneAs()
	{
		PROFILE_FUNCTION();

		std::string file_path = FileDialog::SaveFile("scene(*.scn)\0*.scn\0");
		if (!file_path.empty())
			m_pMainScene->Serializer(file_path);

		/* �ڱ���֮�󣬽���ǰ����·���л�Ϊ�´�����·�� */
		m_ActiveScenePath = file_path;
	}

	void SceneEditorLayer::ShowSceneViewportUI()
	{
		PROFILE_FUNCTION();

		//m_ViewportRegion = { 268, 2188, 317, 1371 };
		static ImGuiWindowFlags tab_bar_flags = ImGuiWindowFlags_NoFocusOnAppearing;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene", &m_IsActivated);
		{
			/* ��ȡ���ڷ�Χ */
			const auto viewport_region_min = ImGui::GetWindowContentRegionMin();
			const auto viewport_region_max = ImGui::GetWindowContentRegionMax();
			const auto viewport_offset = ImGui::GetWindowPos();
			m_ViewportRegion.MinX = viewport_region_min.x + viewport_offset.x;
			m_ViewportRegion.Width = viewport_region_max.x - viewport_region_min.x;
			m_ViewportRegion.MinY = viewport_region_min.y + viewport_offset.y;
			m_ViewportRegion.Height = viewport_region_max.y - viewport_region_min.x;

			/* ����ǰImGui���ڲ��������ڣ�Ӧ�����¼����� */
			m_IsViewportFocused = ImGui::IsWindowFocused();
			m_IsViewportHovered = ImGui::IsWindowHovered();
			// Application::Instance()->GetImGuiLayer()->BlockEvents(!m_IsViewportFocused && !m_IsViewportHovered);

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
					OnDragItemToScene(g_AssetsPath / path);
				}
				ImGui::EndDragDropTarget();
			}

			/* Gizmos */
			ShowOperationGizmoUI();
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void SceneEditorLayer::ShowStatisticInfoUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Stat Info");
		{
			// Renderer3D Stats
			if (ImGui::CollapsingHeader("3D"))
			{
				// todo: �����Ρ�ģ������
			}
		}
		ImGui::End();
	}

	void SceneEditorLayer::ShowOperationGizmoUI()
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
				/* �ӱ任�����лָ� */
				glm::vec3 position, rotation, scale;
				DecomposeTransform(transform_mat, position, rotation, scale);

				/* ���������Ϣ */
				glm::vec3 delta_rotation = rotation - transform_component.Rotation;
				transform_component.Position = position;
				transform_component.Rotation += delta_rotation;
				transform_component.Scale = scale;
			}
		}

		/* �����߿�
		 *
		 * todo: ���߿򻮵���������֮��
		 */
		//const auto identity_mat = glm::identity<glm::mat4>();
		//ImGuizmo::DrawGrid(glm::value_ptr(view_mat), glm::value_ptr(projection_mat), glm::value_ptr(identity_mat), 100.f);

		/* ���ϽǷ�λ��ͼ */
		/*if (!m_pEditorCamera->IsFocus())
		{
			ImGuizmo::ViewManipulate(glm::value_ptr(view_mat), m_pEditorCamera->GetDistance(), ImVec2(m_ViewportRegion.z - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x00000000);
			m_pEditorCamera->SetViewMatrix(view_mat);
		}*/

	}

	void SceneEditorLayer::CheckMouseSelectEntity()
	{
		PROFILE_FUNCTION();

		auto [mouse_x, mouse_y] = ImGui::GetMousePos();
		mouse_x -= m_ViewportRegion.MinX;
		mouse_y -= m_ViewportRegion.MinY;

		const glm::vec2 viewport_size = glm::vec2(m_ViewportRegion.Width, m_ViewportRegion.Height);
		mouse_y = viewport_size.y - mouse_y;

		if (mouse_x > 0 && mouse_y > 0 && mouse_x < viewport_size.x && mouse_y < viewport_size.y)
		{
			int pixel_data = m_pEditorCamera->PickingEntityByPixelPos((int)mouse_x, (int)mouse_y);
			m_HoveredEntity = pixel_data == -1 ? Entity() : Entity((entt::entity)pixel_data, m_pMainScene);
			EDITOR_LOG_DEBUG(pixel_data);
		}
	}

	void SceneEditorLayer::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();

		if (!m_IsActivated)
			return;

		UpdateViewport();

		if (m_ViewportRegion.Width <= 0 || m_ViewportRegion.Height <= 0)
			return;

		Renderer::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		Renderer::Clear();

		if (m_pMainScene)
		{
			switch (m_PlayMode)
			{
			case PlayMode::Edit:
				/* ���������Ϣ */
				if (m_IsViewportFocused)
				{
					m_pEditorCamera->OnUpdate(delta_time);
				}

				/* ���³����е�ʵ�� */
				m_pMainScene->OnUpdateEditor(m_pEditorCamera.get(), delta_time);
				break;
			case PlayMode::Runtime:
				/* ���³��� */
				m_pMainScene->OnUpdateRuntime(delta_time);
				break;
			}
		}
	}
}
