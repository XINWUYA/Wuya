#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace Wuya
{
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		// 默认具有变换组件和标签组件
		entity.AddComponent<TransformComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Tag" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		m_Registry.destroy(entity);
	}

	template <typename T>
	void Scene::OnComponentAdded(Entity& entity, T& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity& entity, TagComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity& entity, TransformComponent& component)
	{
		
	}
}
