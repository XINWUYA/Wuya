#include "EditorSceneHierarchy.h"
#include "EditorUICreator.h"
#include "EditorAssetManager.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

extern const std::filesystem::path g_AssetPath;

EditorSceneHierarchy::EditorSceneHierarchy()
{
	InitIcons();
}

EditorSceneHierarchy::EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene)
{
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
	ImGui::Begin("Properties");
	{
		if (m_SelectedEntity)
			ShowEntityComponents(m_SelectedEntity);
	}
	ImGui::End();
}

void EditorSceneHierarchy::InitIcons()
{
	m_pAddComponentIcon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/add.png");
	m_pMenuIcon = EditorAssetManager::Instance()->GetOrCreateTexture("editor_res/icons/menu.png");
}

void EditorSceneHierarchy::ShowEntityNode( Wuya::Entity& entity)
{
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
static void ShowComponent(const std::string& name, Wuya::Entity& entity, UIFunction function)
{
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
			function(component);
			ImGui::TreePop();
		}

		/* 删除组件 */
		if (remove)
			entity.RemoveComponent<T>();
	}
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 80);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

void EditorSceneHierarchy::ShowEntityComponents(Wuya::Entity& entity)
{
	/* 实体名称组件 */ 
	if (entity.HasComponent<Wuya::NameComponent>())
	{
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
		ShowComponent<Wuya::TransformComponent>("Transform", entity,
			[](auto& component)
			{
				DrawVec3Control("Position", component.Position, 0.0f, 50.0f);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation, 0.0f, 50.0f);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f, 50.0f);

			});
	}

	/* 图片精灵组件 */
	if (entity.HasComponent<Wuya::SpriteComponent>())
	{
		ShowComponent<Wuya::SpriteComponent>("Sprite", entity,
			[](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.BaseColor));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						const std::filesystem::path texture_path = g_AssetPath / path;
						auto texture = EditorAssetManager::Instance()->GetOrCreateTexture(texture_path.string());
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							EDITOR_LOG_WARN("Failed to load texture {0}.", texture_path.filename().string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});
	}

	// String
	std::string val = "aaaaaaaaaaaaaa";
	CREATE_UI("String", "test", &val);
}
