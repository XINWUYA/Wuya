#include "pch.h"
#include "EditorSceneHierarchy.h"
#include "EditorUIFunctions.h"
#include "EditorAssetManager.h"
#include <glm/gtc/type_ptr.hpp>

extern const std::filesystem::path g_AssetPath;

EditorSceneHierarchy::EditorSceneHierarchy()
{
	PROFILE_FUNCTION();

	InitIcons();
}

EditorSceneHierarchy::EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene)
{
	PROFILE_FUNCTION();

	SetOwnerScene(scene);
	InitIcons();
}

void EditorSceneHierarchy::SetOwnerScene(const Wuya::SharedPtr<Wuya::Scene>& scene)
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

void EditorSceneHierarchy::SetSelectedEntity(const Wuya::Entity& entity)
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
				Wuya::Entity entity{ entity_id, m_pOwnerScene.get() };
				ShowEntityNode(entity);
			});

		/* 点击左键，选中 */
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		/* 空白处右键，唤出新建 */
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("New Entity"))
			{
				const auto entity = m_pOwnerScene->CreateEntity("New Entity");
				m_SelectedEntity = entity;
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
			ShowEntityComponents(m_SelectedEntity);
	}
	ImGui::End();
}

void EditorSceneHierarchy::InitIcons()
{
	PROFILE_FUNCTION();

	m_pAddComponentIcon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/add.png");
	m_pMenuIcon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/menu.png");
}

void EditorSceneHierarchy::ShowEntityNode( Wuya::Entity& entity)
{
	PROFILE_FUNCTION();

	const auto& name = entity.GetComponent<Wuya::NameComponent>().Name;

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
static void ShowComponent(const std::string& name, Wuya::Entity& entity, UIFunction show_custom)
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
		const auto menu_icon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/menu.png");
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

void EditorSceneHierarchy::ShowEntityComponents(Wuya::Entity& entity)
{
	PROFILE_FUNCTION();

	/* 实体名称组件 */
	if (entity.HasComponent<Wuya::NameComponent>())
	{
		PROFILE_SCOPE("Show NameComponent");

		auto& name = entity.GetComponent<Wuya::NameComponent>().Name;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		std::strncpy(buffer, name.c_str(), sizeof(buffer));
		ImGui::Text("Name:");
		ImGui::SameLine();
		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			name = std::string(buffer);
		}
	}

	/* 窗口区域宽度 */
	const float panel_width = ImGui::GetContentRegionAvail().x;

	/* 添加组件按钮 */
	ImGui::SameLine(panel_width - 15);
	START_STYLE_ALPHA(0.5f);
	if (ImGui::ImageButton((ImTextureID)m_pAddComponentIcon->GetTextureID(), ImVec2(20, 20), ImVec2(0, 1), ImVec2(1, 0)))
		ImGui::OpenPopup("AddComponentPopup");
	END_STYLE_ALPHA;

	if (ImGui::BeginPopup("AddComponentPopup"))
	{
		/* 相机 */
		if (ImGui::MenuItem("Camera Component"))
		{
			if (m_SelectedEntity.HasComponent<Wuya::CameraComponent>()) /* 为什么不使用entity */
				EDITOR_LOG_WARN("Another camera component has existed!");
			else
				m_SelectedEntity.AddComponent<Wuya::CameraComponent>();

			ImGui::CloseCurrentPopup();
		}

		/* 图片精灵 */
		if (ImGui::MenuItem("Sprite Component"))
		{
			if (m_SelectedEntity.HasComponent<Wuya::SpriteComponent>())
				EDITOR_LOG_WARN("Another sprite component has existed!");
			else
				m_SelectedEntity.AddComponent<Wuya::SpriteComponent>();

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	/* 空间变换组件 */
	if (entity.HasComponent<Wuya::TransformComponent>())
	{
		PROFILE_SCOPE("Show TransformComponent");

		ShowComponent<Wuya::TransformComponent>("Transform", entity,
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
	if (entity.HasComponent<Wuya::SpriteComponent>())
	{
		PROFILE_SCOPE("Show SpriteComonent");

		ShowComponent<Wuya::SpriteComponent>("Sprite", entity,
			[](auto& component)
			{
				EditorUIFunctions::DrawColorUI("BaseColor", component.BaseColor);
				EditorUIFunctions::DrawTextureUI("Texture", component.Texture, component.TilingFactor);
			});
	}
	
}
