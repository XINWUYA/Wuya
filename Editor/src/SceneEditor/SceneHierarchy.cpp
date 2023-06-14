#include "Pch.h"
#include "SceneHierarchy.h"
#include <glm/gtc/type_ptr.hpp>

namespace Wuya
{
	SceneHierarchy::SceneHierarchy()
	{
		PROFILE_FUNCTION();

		InitIcons();
	}

	SceneHierarchy::SceneHierarchy(const SharedPtr<Scene>& scene)
	{
		PROFILE_FUNCTION();

		SetOwnerScene(scene);
		InitIcons();
	}

	void SceneHierarchy::SetOwnerScene(const SharedPtr<Scene>& scene)
	{
		m_pOwnerScene = scene;
		m_SelectedEntity = {};
	}

	void SceneHierarchy::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* ����ʵ���б� */
		ShowSceneHierarchyUI();

		/* ʵ������ */
		ShowEntityPropertiesUI();
	}

	void SceneHierarchy::SetSelectedEntity(const Entity& entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneHierarchy::ShowSceneHierarchyUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Scene Hierarchy");
		{
			m_pOwnerScene->GetRegistry().each(
				[&](auto entity_id)
				{
					Entity entity{ entity_id, m_pOwnerScene };
					ShowEntityNode(entity);
				});

			/* ��������ѡ�� */
			// if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			// 	m_SelectedEntity = {};

			/* �հ״��Ҽ��������½� */
			if (ImGui::BeginPopupContextWindow("New", 1, false))
			{
				if (ImGui::BeginMenu("New A Entity"))
				{
					if (ImGui::MenuItem("Empty"))
					{
						const auto entity = m_pOwnerScene->CreateEntity("Empty");
						m_SelectedEntity = entity;
					}

					if (ImGui::MenuItem("Sprite"))
					{
						auto entity = m_pOwnerScene->CreateEntity("New Sprite");
						entity.AddComponent<SpriteComponent>();
						m_SelectedEntity = entity;
					}

					if (ImGui::MenuItem("Camera"))
					{
						auto entity = m_pOwnerScene->CreateEntity("New Camera");
						entity.AddComponent<CameraComponent>();
						m_SelectedEntity = entity;
					}

					if (ImGui::MenuItem("Directional Light"))
					{
						auto entity = m_pOwnerScene->CreateEntity("New Directional Light");
						entity.AddComponent<LightComponent>(LightType::Directional);
						m_SelectedEntity = entity;
					}

					if (ImGui::MenuItem("Point Light"))
					{
						auto entity = m_pOwnerScene->CreateEntity("New Point Light");
						entity.AddComponent<LightComponent>(LightType::Point);
						m_SelectedEntity = entity;
					}

					if (ImGui::MenuItem("Spot Light"))
					{
						auto entity = m_pOwnerScene->CreateEntity("New Spot Light");
						entity.AddComponent<LightComponent>(LightType::Spot);
						m_SelectedEntity = entity;
					}

					ImGui::EndMenu();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	void SceneHierarchy::ShowEntityPropertiesUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Properties");
		{
			if (m_SelectedEntity)
				ShowEntityComponents();
		}
		ImGui::End();
	}

	void SceneHierarchy::InitIcons()
	{
		PROFILE_FUNCTION();

		m_pAddComponentIcon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/add.png"));
		m_pMenuIcon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/menu.png"));
	}

	void SceneHierarchy::ShowEntityNode(Entity& entity)
	{
		PROFILE_FUNCTION();

		const auto& name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		const bool is_opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		/* �Ҽ�ѡ��ɾ����ǰ�ڵ� */
		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entity_deleted = true;

			ImGui::EndPopup();
		}

		/* չ���ӽڵ� */
		if (is_opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (bool child_opened = ImGui::TreeNodeEx((void*)9817239, flags, name.c_str()))
				ImGui::TreePop();
			ImGui::TreePop();
		}

		/* ȷ��ɾ���ڵ� */
		if (entity_deleted)
		{
			m_pOwnerScene->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	/* ������ */
	template<typename T, typename UIFunction>
	static void ShowComponent(const std::string& name, Entity& entity, UIFunction show_custom)
	{
		PROFILE_FUNCTION();

		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			const float panel_width = ImGui::GetContentRegionAvail().x;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			//float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();

			/* ����ͼ�� */
			START_TRANSPARENT_BUTTON;
			START_STYLE_ALPHA(0.5f);
			ImGui::SameLine(panel_width - 15);
			const auto menu_icon = TextureAssetManager::Instance().GetOrCreateTexture(ABSOLUTE_PATH("EditorRes/icons/menu.png"));
			if (ImGui::ImageButton((ImTextureID)menu_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
				ImGui::OpenPopup("ComponentSettings");
			END_STYLE_ALPHA;
			END_TRANSPARENT_BUTTON;

			/* ɾ�����ѡ�� */
			bool remove = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove"))
					remove = true;

				ImGui::EndPopup();
			}

			/* չ��ʱ��ʾ������� */
			if (open)
			{
				show_custom(component);
				ImGui::TreePop();
			}

			/* ɾ����� */
			if (remove)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchy::ShowEntityComponents()
	{
		PROFILE_FUNCTION();

		/* ʵ��������� */
		ShowNameComponent();

		/* ���������ť */
		const float panel_width = ImGui::GetContentRegionAvail().x; /* ���������� */
		ImGui::SameLine(panel_width - 15); /* ������ͬ�п��ҵ�λ�� */
		ShowAddComponentButton();

		/* �ռ�任��� */
		ShowTransformComponent();

		/* ͼƬ������� */
		ShowSpriteComponent();

		/* ���������� */
		ShowCameraComponent();

		/* ģ����� */
		ShowModelComponent();

		/* ��Դ��� */
		ShowLightComponent();
	}

	/* ʵ��������� */
	void SceneHierarchy::ShowNameComponent()
	{
		PROFILE_FUNCTION();

		if (m_SelectedEntity.HasComponent<NameComponent>())
		{
			PROFILE_SCOPE("Show NameComponent");

			auto& name = m_SelectedEntity.GetComponent<NameComponent>().Name;

			char buffer[256] = {};
			std::strncpy(buffer, name.c_str(), sizeof(buffer));
			ImGui::Text("Name:");
			ImGui::SameLine();
			if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}
	}

	/* ���������ť */
	void SceneHierarchy::ShowAddComponentButton()
	{
		PROFILE_FUNCTION();

		START_STYLE_ALPHA(0.5f);
		if (ImGui::ImageButton((ImTextureID)m_pAddComponentIcon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
			ImGui::OpenPopup("AddComponentPopup");
		END_STYLE_ALPHA;

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			/* ��� */
			if (ImGui::MenuItem("Camera Component"))
			{
				m_SelectedEntity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* ͼƬ���� */
			if (ImGui::MenuItem("Sprite Component"))
			{
				m_SelectedEntity.AddComponent<SpriteComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* ģ�� */
			if (ImGui::MenuItem("Model Component"))
			{
				m_SelectedEntity.AddComponent<ModelComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* ��Դ */
			if (ImGui::MenuItem("Light Component"))
			{
				m_SelectedEntity.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	/* �ռ�任��� */
	void SceneHierarchy::ShowTransformComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<TransformComponent>("Transform", m_SelectedEntity,
			[](auto& component)
			{
				ImGuiExt::DrawVec3ControlUI("Position", component.Position, 0.0f);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				ImGuiExt::DrawVec3ControlUI("Rotation", rotation, 0.0f);
				component.Rotation = glm::radians(rotation);
				ImGuiExt::DrawVec3ControlUI("Scale", component.Scale, 1.0f);

			});
	}

	/* ͼƬ������� */
	void SceneHierarchy::ShowSpriteComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<SpriteComponent>("Sprite", m_SelectedEntity,
			[](auto& component)
			{
				ImGuiExt::DrawColorUI("BaseColor", component.BaseColor);
				ImGuiExt::DrawTextureUI("Texture", component.Texture, component.TilingFactor);
			});
	}

	/* ���������� */
	void SceneHierarchy::ShowCameraComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<CameraComponent>("Camera", m_SelectedEntity,
			[](auto& component)
			{
				ImGuiExt::DrawCheckboxUI("IsPrimary", component.IsPrimary);
				ImGuiExt::DrawCheckboxUI("IsFixedAspectRatio", component.IsFixedAspectRatio);
				auto& scene_camera = component.Camera;
				int projection_idx = static_cast<int>(scene_camera->GetProjectionType());
				ImGuiExt::DrawComboUI("ProjectionType", GetEnumNames<SceneCamera::ProjectionType>(), projection_idx,
					[&scene_camera](int selected_idx)
					{
						scene_camera->SetProjectionType(static_cast<SceneCamera::ProjectionType>(selected_idx));
					});

				switch (scene_camera->GetProjectionType())
				{
				case SceneCamera::ProjectionType::Perspective:
				{
					const auto& camera_desc = scene_camera->GetPerspectiveCameraDesc();

					float fov = camera_desc->Fov;
					ImGuiExt::DrawDragFloatUI("Fov", fov);
					float near_clip = camera_desc->Near;
					ImGuiExt::DrawDragFloatUI("Near", near_clip);
					float far_clip = camera_desc->Far;
					ImGuiExt::DrawDragFloatUI("Far", far_clip);

					scene_camera->SetPerspectiveCameraDesc({ fov, near_clip, far_clip });
				}
				break;
				case SceneCamera::ProjectionType::Orthographic:
				{
					const auto& camera_desc = scene_camera->GetOrthographicCameraDesc();

					float height_size = camera_desc->HeightSize;
					ImGuiExt::DrawDragFloatUI("HeightSize", height_size);
					float near_clip = camera_desc->Near;
					ImGuiExt::DrawDragFloatUI("Near", near_clip);
					float far_clip = camera_desc->Far;
					ImGuiExt::DrawDragFloatUI("Far", far_clip);

					scene_camera->SetOrthographicCameraDesc({ height_size, near_clip, far_clip });
				}
				break;
				}
			});
	}

	/* ģ����� */
	void SceneHierarchy::ShowModelComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<ModelComponent>("Model", m_SelectedEntity,
			[](auto& component)
			{
				auto& model = component.Model;

				{
					ImGui::PushID("ModelPath");
					ImGui::Columns(2);

					/* Label */
					ImGui::SetColumnWidth(0, 100);
					ImGui::Text("ModelPath");

					/* Texture */
					ImGui::NextColumn();
					{
						ImGui::TextWrapped(model->GetPath().c_str());
					}

					ImGui::Columns(1);
					ImGui::PopID();
				}

				/*const auto& mesh_segments = model->GetMeshSegments();
				for (auto& mesh_segment : mesh_segments)
				{
					auto* mesh_segment_root = model_root->InsertNewChildElement("MeshSegment");
					mesh_segment_root->SetAttribute("Name", mesh_segment->GetName().c_str());
					auto* mtl_root = mesh_segment_root->InsertNewChildElement("Material");
					mtl_root->SetAttribute("ShaderPath", mesh_segment->GetMaterial()->GetShader()->GetPath().c_str());
				}*/
			});
	}

	/* ��Դ��� */
	void SceneHierarchy::ShowLightComponent()
	{
		PROFILE_FUNCTION();
		ShowComponent<LightComponent>("Light", m_SelectedEntity,
			[](auto& component)
			{
				SharedPtr<Light>& light = component.Light;
				if (!light)
					return;

				/* ��Դ���� */
				int type_idx = static_cast<int>(light->GetLightType());
				ImGuiExt::DrawComboUI("Type", GetEnumNames<LightType>(), type_idx,
					[&light](int selected_idx)
					{
						/* todo: �л���Դ���� */
						//scene_camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(selected_idx));
					});

				/* ��Դ��ɫ */
				glm::vec4 light_color = light->GetColor();
				ImGuiExt::DrawColorUI("Color", light_color);
				light->SetColor(light_color);

				/* ��Դǿ�� */
				float light_intensity = light->GetIntensity();
				ImGuiExt::DrawDragFloatUI("Intensity", light_intensity);
				light->SetIntensity(light_intensity);

				/* ͶӰ */
				bool cast_shadow = light->IsCastShadow();
				ImGuiExt::DrawCheckboxUI("CastShadow", cast_shadow);
				light->SetIsCastShadow(cast_shadow);
			});
	}
}
