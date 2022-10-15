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

		/* 默认增加一盏方向光 */
		Entity entity = m_pDefaultScene->CreateEntity("Light");
		auto& light_component = entity.AddComponent<LightComponent>(LightType::Directional);
		light_component.Light->SetColor(glm::vec4(1, 0, 0, 1));
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

		if (m_ViewportRegion.Width > 0 && m_ViewportRegion.Height > 0/* && (desc.Width != m_ViewportRegion.Width() || desc.Height != m_ViewportRegion.Height())*/)
		{
			//m_pFrameBuffer->Resize(m_ViewportRegion.Width(), m_ViewportRegion.Height());
			m_pEditorCamera->SetViewportRegion({ 0,0,m_ViewportRegion.Width, m_ViewportRegion.Height });
			//m_pOrthographicCameraController->OnResize(static_cast<float>(m_ViewportRegion.Width()), static_cast<float>(m_ViewportRegion.Height()));
		}
		m_pEditorCamera->OnUpdate(delta_time);
		m_pDefaultScene->OnUpdateEditor(m_pEditorCamera.get(), delta_time);
	}

	void ModelEditorLayer::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* 显示菜单栏UI */
		ShowMenuUI();
		/* 显示主场景视口 */
		ShowSceneViewportUI();
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

					if (ImGui::MenuItem("Export Model(.mesh)", "Ctrl+O"))
					{
						ExportModel();
					}

					if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					{
						//SaveScene();
					}

					if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))
					{
						//SaveSceneAs();
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

	/* 导入模型 */
	void ModelEditorLayer::ImportModel()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("Obj(*.obj)\0*.obj\0");
		if (!file_path.empty())
		{
			/* 先移除场景中的其他Entity */
			m_pDefaultScene->DestroyTargetEntities<ModelComponent>();
			/* 将模型添加到场景中 */
			const std::string file_name = ExtractFilename(file_path);
			Entity entity = m_pDefaultScene->CreateEntity(file_name);
			auto& mesh_component = entity.AddComponent<ModelComponent>();
			mesh_component.Model = Model::Create(file_path);
			/* todo: 根据模型大小自适应相机距离 */

			m_ModelPath = file_path;
		}
	}

	/* 导出模型 */
	void ModelEditorLayer::ExportModel()
	{
	}
}
