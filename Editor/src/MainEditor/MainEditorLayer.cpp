#include "Pch.h"
#include "MainEditorLayer.h"
#include <glm/gtc/type_ptr.inl>
#include "EditorBuiltinCamera.h"
#include "EditorUIFunctions.h"
#include "ImGuizmo.h"
#include "ModelEditor/ModelEditorLayer.h"
#include "SceneEditor/SceneEditorLayer.h"

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	MainEditorLayer::MainEditorLayer()
		: ILayer("MainEditorLayer")
	{
		PROFILE_FUNCTION();
	}

	void MainEditorLayer::OnAttached()
	{
		PROFILE_FUNCTION();
		
	}

	void MainEditorLayer::OnDetached()
	{
		PROFILE_FUNCTION();

		ILayer::OnDetached();
	}

	void MainEditorLayer::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* �˵� */
		ShowMenuUI();
		/* ��������UI */
		ShowSceneControllerUI();
		/* ��Դ������ */
		m_ResourceBrowser.OnImGuiRenderer();

		// bool show = true;
		// ImGui::ShowDemoWindow(&show);
	}

	void MainEditorLayer::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		if (!event)
			return;

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(MainEditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNC(MainEditorLayer::OnMouseButtonPressed));
	}

	bool MainEditorLayer::OnKeyPressed(KeyPressedEvent* event)
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
			break;
		case Key::S: /* Ctrl+S: ���泡��; Ctrl+Shift+S: �������Ϊ */
			if (is_ctrl_pressed)
			{
				if (is_shift_pressed)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;
		case Key::I: /* Ctrl+I������һ������ */
			{
				if (is_ctrl_pressed)
					ImportScene();
			}
			break;
		case Key::Q:
			{
				if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				{
					m_GizmoType = -1;
					NotifyGizmoTypeChanged();
				}
			}
			break;
		case Key::W:
			{
				if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				{
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					NotifyGizmoTypeChanged();
				}
			}
			break;
		case Key::E:
			{
				if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				{
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					NotifyGizmoTypeChanged();
				}
			}
			break;
		case Key::R:
			{
				if (!ImGuizmo::IsUsing() && !is_button_right_pressed)
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
					NotifyGizmoTypeChanged();
				}
			}
			break;
		}

		return true;
	}

	bool MainEditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent* event)
	{
		PROFILE_FUNCTION();
		return false;
	}
	
	void MainEditorLayer::NewScene()
	{
		PROFILE_FUNCTION();

		const auto& scene_editor_layer = reinterpret_cast<const SharedPtr<SceneEditorLayer>&>(Application::Instance()->GetLayerByName("SceneEditorLayer"));
		scene_editor_layer->NewScene();
	}

	void MainEditorLayer::ImportScene()
	{
		PROFILE_FUNCTION();

		const auto file_path = FileDialog::OpenFile("scene(*.scn)\0*.scn\0");
		if (!file_path.empty())
		{
		}
	}

	void MainEditorLayer::SaveScene()
	{
		PROFILE_FUNCTION();
	}

	/* ���泡����ָ��·�� */
	void MainEditorLayer::SaveSceneAs()
	{
		PROFILE_FUNCTION();

		std::string file_path = FileDialog::SaveFile("scene(*.scn)\0*.scn\0");

	}

	void MainEditorLayer::ShowMenuUI()
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

			/* �˵��� */
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
						ImportScene();
					}

					if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					{
						SaveScene();
					}

					if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))
					{
						SaveSceneAs();
					}

					if (ImGui::MenuItem("Import Model(.obj)", "Ctrl+I"))
					{
						m_ActiveModelEditor = true;
						const auto& model_editor_layer = reinterpret_cast<const SharedPtr<ModelEditorLayer>&>(Application::Instance()->GetLayerByName("ModelEditorLayer"));
						model_editor_layer->Active(m_ActiveModelEditor);
						model_editor_layer->ImportModel();
					}

					if (ImGui::MenuItem("Export Mesh & Mtl(.mesh & .mtl)", "Ctrl+E"))
					{
						//ExportMeshAndMtl();
						const auto& scene_editor_layer = reinterpret_cast<const SharedPtr<SceneEditorLayer>&>(Application::Instance()->GetLayerByName("SceneEditorLayer"));
						static bool active = false;
						scene_editor_layer->Active(active);
						active = !active;
					}

					if (ImGui::MenuItem("Exit"))
						Application::Instance()->Close();

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Windows"))
				{
					const auto& scene_editor_layer = reinterpret_cast<const SharedPtr<SceneEditorLayer>&>(Application::Instance()->GetLayerByName("SceneEditorLayer"));
					m_ActiveSceneEditor = scene_editor_layer->IsActivated();
					if (ImGui::MenuItem("SceneEditor", NULL, &m_ActiveSceneEditor))
					{
						scene_editor_layer->Active(m_ActiveSceneEditor);
					}

					const auto& model_editor_layer = reinterpret_cast<const SharedPtr<ModelEditorLayer>&>(Application::Instance()->GetLayerByName("ModelEditorLayer"));
					m_ActiveModelEditor = model_editor_layer->IsActivated();
					if (ImGui::MenuItem("ModelEditor", NULL, &m_ActiveModelEditor))
					{
						model_editor_layer->Active(m_ActiveModelEditor);
					}

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

	/* ��ʾ��������UI */
	void MainEditorLayer::ShowSceneControllerUI()
	{
		PROFILE_FUNCTION();

		const TextureLoadConfig load_config;
		static auto save_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/save.png", load_config);
		static auto translate_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/translate.png", load_config);
		static auto rotate_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/rotate.png", load_config);
		static auto scale_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/scale.png", load_config);
		static auto play_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/play.png", load_config);
		static auto stop_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/stop.png", load_config);
		static auto menu_icon = TextureAssetManager::Instance().GetOrCreateTexture("editor_res/icons/menu.png", load_config);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2)); /* ָ����� */
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 2));

		ImGui::Begin("##Scene Controller", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{
			const float icon_size = ImGui::GetWindowHeight() - 4.0f;
			const float panel_width = ImGui::GetWindowContentRegionMax().x;

			START_TRANSPARENT_BUTTON;

			constexpr float cursor_offset = 10.0f;
			/* ���水ť */
			ImGui::SetCursorPosX(cursor_offset);
			/*if (ImGui::ImageButton((ImTextureID)save_icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
				SaveScene();*/
			bool checked = false;
			EditorUIFunctions::DrawCheckedImageButtonUI("Save", save_icon, ImVec2(icon_size, icon_size), checked,
				[&]()
				{
					SaveScene();
				});

			/* �ƶ�/��ת/ƽ�Ʋ��� */
			{
				/* translate */
				ImGui::SameLine(cursor_offset + icon_size * 2);
				bool checked = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Translate", translate_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
						NotifyGizmoTypeChanged();
					});

				/* rotate */
				ImGui::SameLine();
				checked = m_GizmoType == ImGuizmo::OPERATION::ROTATE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Rotate", rotate_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::ROTATE;
						NotifyGizmoTypeChanged();
					});

				/* scale */

				ImGui::SameLine();
				checked = m_GizmoType == ImGuizmo::OPERATION::SCALE;
				EditorUIFunctions::DrawCheckedImageButtonUI("Scale", scale_icon, ImVec2(icon_size, icon_size), checked,
					[&]()
					{
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
						NotifyGizmoTypeChanged();
					});
			}

			/* �л�ִ��ģʽ */
			{
				ImGui::SameLine();
				const SharedPtr<Texture> icon = (m_PlayMode == PlayMode::Edit) ? play_icon : stop_icon;
				ImGui::SetCursorPosX((panel_width - icon_size) * 0.5f);
				if (ImGui::ImageButton((ImTextureID)icon->GetTextureID(), ImVec2(icon_size, icon_size), ImVec2(0, 1), ImVec2(1, 0), 0))
				{
					m_PlayMode = (m_PlayMode == PlayMode::Edit) ? PlayMode::Runtime : PlayMode::Edit;
					NotifyPlayModeChanged();
				}
			}

			/* ���� */
			{
				ImGui::SameLine(panel_width - cursor_offset - 20);
				START_STYLE_ALPHA(0.5f);
				if (ImGui::ImageButton((ImTextureID)menu_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
					ImGui::OpenPopup("ConfigPopup");
				END_STYLE_ALPHA;

				/* չ������ʱ����ʾ�ؼ� */
				// if (ImGui::BeginPopup("ConfigPopup"))
				// {
				// 	ImGui::PushItemWidth(200);
				//
				// 	bool is_focus = m_pEditorCamera->IsFocus();
				// 	ImGui::Checkbox("FocusMode", &is_focus);
				// 	m_pEditorCamera->SetFocus(is_focus);
				//
				// 	ImGui::PopItemWidth();
				// 	ImGui::EndPopup();
				// }
			}

			END_TRANSPARENT_BUTTON;
		}

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	/* ��ʾ��Ⱦͳ����Ϣ */
	void MainEditorLayer::ShowStatisticInfoUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Stat Info");
		{
			// todo: ֡�ʵ�
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
				// todo: �����Ρ�ģ������
			}
		}
		ImGui::End();
	}

	void MainEditorLayer::NotifyGizmoTypeChanged()
	{
		const auto& scene_editor_layer = reinterpret_cast<const SharedPtr<SceneEditorLayer>&>(Application::Instance()->GetLayerByName("SceneEditorLayer"));
		scene_editor_layer->SetGizmoType(m_GizmoType);
	}

	void MainEditorLayer::NotifyPlayModeChanged()
	{
		const auto& scene_editor_layer = reinterpret_cast<const SharedPtr<SceneEditorLayer>&>(Application::Instance()->GetLayerByName("SceneEditorLayer"));
		scene_editor_layer->SetPlayMode(m_PlayMode);
	}

	void MainEditorLayer::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();
		
	}
}
