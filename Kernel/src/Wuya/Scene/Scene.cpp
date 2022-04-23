#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Wuya/Renderer/Renderer2D.h"
#include "Wuya/Application/EditorCamera.h"
#include <tinyxml2.h>

#include "Wuya/Common/Utils.h"

namespace Wuya
{
	Entity Scene::CreateEntity(const std::string& name)
	{
		PROFILE_FUNCTION();

		Entity entity = { m_Registry.create(), this };

		/* 默认添加名称组件和变换组件 */
		auto& name_component = entity.AddComponent<NameComponent>();
		name_component.Name = name.empty() ? "Unnamed Entity" : name;

		entity.AddComponent<TransformComponent>();

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		PROFILE_FUNCTION();

		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(float delta_time)
	{
		PROFILE_FUNCTION();

		/* 更新所有实体的变换 */
		// auto entity_group = m_Registry.group<TransformComponent>(entt::get<>)
	}

	void Scene::OnUpdateEditor(const SharedPtr<EditorCamera>& camera, float delta_time)
	{
		PROFILE_FUNCTION();

		/* 更新Renderer2D信息 */
		Renderer2D::BeginScene(camera);

		/* 更新所有图片实体 */
		const auto entity_group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto& entity : entity_group)
		{
			auto [transform_component, sprite_component] = entity_group.get<TransformComponent, SpriteComponent>(entity);
			Renderer2D::DrawSprite(transform_component.GetTransform(), &sprite_component, static_cast<int>(entity));
		}

		/* 提交绘制数据 */
		Renderer2D::EndScene();
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		PROFILE_FUNCTION();

		/* difference of view and group: https://github.com/skypjack/entt/discussions/638 */
		const auto entity_view = m_Registry.view<CameraComponent>();
		for (auto& entity : entity_view)
		{
			const auto& camera_component = entity_view.get<CameraComponent>(entity);
			if (camera_component.IsPrimary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::Serializer(const std::string& path)
	{
		PROFILE_FUNCTION();

		auto* doc = new tinyxml2::XMLDocument();
		doc->InsertEndChild(doc->NewDeclaration());
		tinyxml2::XMLElement* scene_root = doc->NewElement("Scene");
		doc->InsertEndChild(scene_root);

		tinyxml2::XMLElement* entities_root = scene_root->InsertNewChildElement("Entities");

		/* 遍历场景中实体，进行序列化 */
		m_Registry.each(
			[&](auto& entity_id)
			{
				Entity entity = { entity_id, this };
				if (!entity)
					return;

				SerializeEntity(entities_root, entity);
			}
		);

		/* 保存到文本 */
		doc->SaveFile(path.c_str());
	}

	bool Scene::Deserializer(const std::string& path)
	{
		PROFILE_FUNCTION();

		auto* doc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError error = doc->LoadFile(path.c_str());
		if (error != tinyxml2::XML_SUCCESS)
		{
			CORE_LOG_ERROR("Failed to deserializer scene file: {}.", path);
			return false;
		}

		tinyxml2::XMLElement* scene_root = doc->FirstChildElement("Scene");
		tinyxml2::XMLElement* entities_root = scene_root->FirstChildElement("Entities");

		for (tinyxml2::XMLElement* entity_root = entities_root->FirstChildElement(); entity_root; entity_root = entity_root->NextSiblingElement("Entity"))
		{
			if (auto* id_attr = entity_root->FindAttribute("ID"))
			{
				int entity_id = id_attr->IntValue();

				/* Name */
				std::string name;
				if (auto* name_attr = entity_root->FindAttribute("Name"))
					name = name_attr->Value();

				if (!name.empty())
				{
					auto entity = CreateEntity(name);

					/* Transform */
					if (auto* transform_root = entity_root->FirstChildElement("Transform"))
					{
						auto& transform_component = entity.GetComponent<TransformComponent>();
						transform_component.Position = ToVec3(transform_root->Attribute("Position"));
						transform_component.Rotation = ToVec3(transform_root->Attribute("Rotation"));
						transform_component.Scale = ToVec3(transform_root->Attribute("Scale"));
					}

					/* */
				}
			}	
		}
		return true;
	}

	void Scene::SerializeEntity(tinyxml2::XMLElement* root_node, Entity& entity)
	{
		PROFILE_FUNCTION();

		tinyxml2::XMLElement* entity_root = root_node->InsertNewChildElement("Entity");

		/* ID */
		entity_root->SetAttribute("ID", (int)entity);

		/* Name */
		if (entity.HasComponent<NameComponent>())
		{
			const auto& component = entity.GetComponent<NameComponent>();
			entity_root->SetAttribute("Name", component.Name.c_str());
		}

		/* Transform */
		if (entity.HasComponent<TransformComponent>())
		{
			auto* transform_root = entity_root->InsertNewChildElement("Transform");
			const auto& component = entity.GetComponent<TransformComponent>();
			transform_root->SetAttribute("Position", ToString(component.Position).c_str());
			transform_root->SetAttribute("Rotation", ToString(component.Rotation).c_str());
			transform_root->SetAttribute("Scale", ToString(component.Scale).c_str());
		}

		/* Sprite */
	}

	template <typename T>
	void Scene::OnComponentAdded(Entity& entity, T& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NameComponent>(Entity& entity, NameComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity& entity, TransformComponent& component)
	{
		
	}
}
