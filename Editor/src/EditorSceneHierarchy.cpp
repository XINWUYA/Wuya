#include "Pch.h"
#include "EditorSceneHierarchy.h"
#include "EditorUIFunctions.h"
#include "EditorAssetManager.h"
#include <glm/gtc/type_ptr.hpp>

namespace Wuya
{
	extern const std::filesystem::path g_AssetPath;

	EditorSceneHierarchy::EditorSceneHierarchy()
	{
		PROFILE_FUNCTION();

		InitIcons();
	}

	EditorSceneHierarchy::EditorSceneHierarchy(const SharedPtr<Scene>& scene)
	{
		PROFILE_FUNCTION();

		SetOwnerScene(scene);
		InitIcons();
	}

	void EditorSceneHierarchy::SetOwnerScene(const SharedPtr<Scene>& scene)
	{
		m_pOwnerScene = scene;
		m_SelectedEntity = {};
	}

	void EditorSceneHierarchy::OnImGuiRender()
	{
		PROFILE_FUNCTION();

		/* 场景实体列表 */
		ShowSceneHierarchyUI();

		/* 实体属性 */
		ShowEntityPropertiesUI();
	}

	void EditorSceneHierarchy::SetSelectedEntity(const Entity& entity)
	{
		m_SelectedEntity = entity;
	}

	void EditorSceneHierarchy::ShowSceneHierarchyUI()
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

			/* 点击左键，选中 */
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectedEntity = {};

			/* 空白处右键，唤出新建 */
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

	void EditorSceneHierarchy::ShowEntityPropertiesUI()
	{
		PROFILE_FUNCTION();

		ImGui::Begin("Properties");
		{
			if (m_SelectedEntity)
				ShowEntityComponents();
		}
		ImGui::End();
	}

	void EditorSceneHierarchy::InitIcons()
	{
		PROFILE_FUNCTION();

		m_pAddComponentIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/add.png");
		m_pMenuIcon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/menu.png");
	}

	void EditorSceneHierarchy::ShowEntityNode(Entity& entity)
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

		/* 右键选择删除当前节点 */
		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entity_deleted = true;

			ImGui::EndPopup();
		}

		/* 展开子节点 */
		if (is_opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (bool child_opened = ImGui::TreeNodeEx((void*)9817239, flags, name.c_str()))
				ImGui::TreePop();
			ImGui::TreePop();
		}

		/* 确认删除节点 */
		if (entity_deleted)
		{
			m_pOwnerScene->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	/* 组件框架 */
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

			/* 设置图标 */
			START_TRANSPARENT_BUTTON;
			START_STYLE_ALPHA(0.5f);
			ImGui::SameLine(panel_width - 15);
			const auto menu_icon = EditorAssetManager::Instance().GetOrCreateTexture("editor_res/icons/menu.png");
			if (ImGui::ImageButton((ImTextureID)menu_icon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
				ImGui::OpenPopup("ComponentSettings");
			END_STYLE_ALPHA;
			END_TRANSPARENT_BUTTON;

			/* 删除组件选项 */
			bool remove = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove"))
					remove = true;

				ImGui::EndPopup();
			}

			/* 展开时显示组件内容 */
			if (open)
			{
				show_custom(component);
				ImGui::TreePop();
			}

			/* 删除组件 */
			if (remove)
				entity.RemoveComponent<T>();
		}
	}

	void EditorSceneHierarchy::ShowEntityComponents()
	{
		PROFILE_FUNCTION();

		/* 实体名称组件 */
		ShowNameComponent();

		/* 增加组件按钮 */
		const float panel_width = ImGui::GetContentRegionAvail().x; /* 窗口区域宽度 */
		ImGui::SameLine(panel_width - 15); /* 放置在同行靠右的位置 */
		ShowAddComponentButton();

		/* 空间变换组件 */
		ShowTransformComponent();

		/* 图片精灵组件 */
		ShowSpriteComponent();

		/* 场景相机组件 */
		ShowCameraComponent();

		/* 模型组件 */
		ShowModelComponent();

		/* 光源组件 */
		ShowLightComponent();
	}

	/* 实体名称组件 */
	void EditorSceneHierarchy::ShowNameComponent()
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

	/* 增加组件按钮 */
	void EditorSceneHierarchy::ShowAddComponentButton()
	{
		PROFILE_FUNCTION();

		START_STYLE_ALPHA(0.5f);
		if (ImGui::ImageButton((ImTextureID)m_pAddComponentIcon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
			ImGui::OpenPopup("AddComponentPopup");
		END_STYLE_ALPHA;

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			/* 相机 */
			if (ImGui::MenuItem("Camera Component"))
			{
				m_SelectedEntity.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* 图片精灵 */
			if (ImGui::MenuItem("Sprite Component"))
			{
				m_SelectedEntity.AddComponent<SpriteComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* 模型 */
			if (ImGui::MenuItem("Model Component"))
			{
				m_SelectedEntity.AddComponent<ModelComponent>();
				ImGui::CloseCurrentPopup();
			}

			/* 光源 */
			if (ImGui::MenuItem("Light Component"))
			{
				m_SelectedEntity.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	/* 空间变换组件 */
	void EditorSceneHierarchy::ShowTransformComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<TransformComponent>("Transform", m_SelectedEntity,
			[](auto& component)
			{
				EditorUIFunctions::DrawVec3ControlUI("Position", component.Position, 0.0f);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				EditorUIFunctions::DrawVec3ControlUI("Rotation", rotation, 0.0f);
				component.Rotation = glm::radians(rotation);
				EditorUIFunctions::DrawVec3ControlUI("Scale", component.Scale, 1.0f);

			});
	}

	/* 图片精灵组件 */
	void EditorSceneHierarchy::ShowSpriteComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<SpriteComponent>("Sprite", m_SelectedEntity,
			[](auto& component)
			{
				EditorUIFunctions::DrawColorUI("BaseColor", component.BaseColor);
				EditorUIFunctions::DrawTextureUI("Texture", component.Texture, component.TilingFactor);
			});
	}

	/* 场景相机组件 */
	void EditorSceneHierarchy::ShowCameraComponent()
	{
		PROFILE_FUNCTION();

		ShowComponent<CameraComponent>("Camera", m_SelectedEntity,
			[](auto& component)
			{
				EditorUIFunctions::DrawCheckboxUI("IsPrimary", component.IsPrimary);
				EditorUIFunctions::DrawCheckboxUI("IsFixedAspectRatio", component.IsFixedAspectRatio);
				auto& scene_camera = component.Camera;
				int projection_idx = static_cast<int>(scene_camera->GetProjectionType());
				EditorUIFunctions::DrawComboUI("ProjectionType", { "Perspective", "Orthographic" }, projection_idx,
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
					EditorUIFunctions::DrawDragFloatUI("Fov", fov);
					float near_clip = camera_desc->Near;
					EditorUIFunctions::DrawDragFloatUI("Near", near_clip);
					float far_clip = camera_desc->Far;
					EditorUIFunctions::DrawDragFloatUI("Far", far_clip);

					scene_camera->SetPerspectiveCameraDesc({ fov, near_clip, far_clip });
				}
				break;
				case SceneCamera::ProjectionType::Orthographic:
				{
					const auto& camera_desc = scene_camera->GetOrthographicCameraDesc();

					float height_size = camera_desc->HeightSize;
					EditorUIFunctions::DrawDragFloatUI("HeightSize", height_size);
					float near_clip = camera_desc->Near;
					EditorUIFunctions::DrawDragFloatUI("Near", near_clip);
					float far_clip = camera_desc->Far;
					EditorUIFunctions::DrawDragFloatUI("Far", far_clip);

					scene_camera->SetOrthographicCameraDesc({ height_size, near_clip, far_clip });
				}
				break;
				}
			});
	}

	/* 模型组件 */
	void EditorSceneHierarchy::ShowModelComponent()
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

	/* 光源组件 */
	void EditorSceneHierarchy::ShowLightComponent()
	{
		PROFILE_FUNCTION();
		ShowComponent<LightComponent>("Light", m_SelectedEntity,
			[](auto& component)
			{
				SharedPtr<Light>& light = component.Light;
				if (!light)
					return;

				/* 光源类型 */
				int type_idx = static_cast<int>(light->GetLightType());
				EditorUIFunctions::DrawComboUI("Type", { "Directional", "Point", "Spot", "Area", "Volume"}, type_idx,
					[&light](int selected_idx)
					{
						/* todo: 切换光源类型 */
						//scene_camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(selected_idx));
					});

				/* 光源颜色 */
				EditorUIFunctions::DrawColorUI("Color", component.Color);
				light->SetColor(component.Color);

				/* 光源强度 */
				EditorUIFunctions::DrawDragFloatUI("Intensity", component.Intensity);
				light->SetIntensity(component.Intensity);

				/* 投影 */
				EditorUIFunctions::DrawCheckboxUI("CastShadow", component.IsCastShadow);
				light->SetIsCastShadow(component.IsCastShadow);
			});
	}
}
