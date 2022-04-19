#include "EditorSceneHierarchy.h"
#include "EditorUICreator.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

extern const std::filesystem::path g_AssetPath;

EditorSceneHierarchy::EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene)
{
	SetOwnerScene(scene);
}

void EditorSceneHierarchy::SetOwnerScene(const Wuya::SharedPtr<Wuya::Scene>& scene)
{
	m_pOwnerScene = scene;
	m_SelectedEntity = {};
}

void EditorSceneHierarchy::OnImGuiRender()
{
	// ����ʵ���б�
	ImGui::Begin("Scene Hierarchy");
	{
		m_pOwnerScene->GetRegistry().each(
			[&](auto entity_id)
			{
				Wuya::Entity entity{ entity_id, m_pOwnerScene.get() };
				ShowEntityNode(entity);
			});

		// ��������ѡ��
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// �հ״��Ҽ��������½�
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

	// ʵ������
	ImGui::Begin("Properties");
	{
		if (m_SelectedEntity)
			ShowEntityComponents(m_SelectedEntity);
	}
	ImGui::End();
}

void EditorSceneHierarchy::SetSelectedEntity(const Wuya::Entity& entity)
{
	m_SelectedEntity = entity;
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


template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Wuya::Entity& entity, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (entity.HasComponent<T>())
	{
		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar(
		);
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if (removeComponent)
			entity.RemoveComponent<T>();
	}
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
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
	//if (ImGui::CollapsingHeader("Base"))
	{
		/* ʵ������� */ 
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

		/* ��������ť */
		ImGui::SameLine();
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPopup");

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			/* ��� */
			if (ImGui::MenuItem("Camera Component"))
			{
				if (m_SelectedEntity.HasComponent<Wuya::CameraComponent>()) /* Ϊʲô��ʹ��entity */
					EDITOR_LOG_WARN("Another camera component has existed!");
				else
					m_SelectedEntity.AddComponent<Wuya::CameraComponent>();

				ImGui::CloseCurrentPopup();
			}

			/* ͼƬ���� */
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

		/* �ռ�任��� */
		if (entity.HasComponent<Wuya::TransformComponent>())
		{
			DrawComponent<Wuya::TransformComponent>("Translation", entity, 
				[](auto& component)
				{
					DrawVec3Control("Position", component.Translation, 0.0f, 50.0f);
					glm::vec3 rotation = glm::degrees(component.Rotation);
					DrawVec3Control("Rotation", rotation, 0.0f, 50.0f);
					component.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", component.Scale, 1.0f, 50.0f);

				});
		}

		/* ͼƬ������� */
		if (entity.HasComponent<Wuya::SpriteComponent>())
		{
			DrawComponent<Wuya::SpriteComponent>("Sprite", entity, 
				[](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.BaseColor));

					ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							auto texture = Wuya::Texture2D::Create(texturePath.string());
							if (texture->IsLoaded())
								component.Texture = texture;
							else
								EDITOR_LOG_WARN("Failed to load texture {0}.", texturePath.filename().string());
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
}
