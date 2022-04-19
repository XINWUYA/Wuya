#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Wuya/Renderer/Renderer2D.h"
#include "Wuya/Application/EditorCamera.h"

namespace Wuya
{
	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		/* Ĭ�������������ͱ任��� */
		auto& name_component = entity.AddComponent<NameComponent>();
		name_component.Name = name.empty() ? "Unnamed Entity" : name;

		entity.AddComponent<TransformComponent>();

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(float delta_time)
	{
		/* ��������ʵ��ı任 */
		// auto entity_group = m_Registry.group<TransformComponent>(entt::get<>)
	}

	void Scene::OnUpdateEditor(const SharedPtr<EditorCamera>& camera, float delta_time)
	{
		/* ����Renderer2D��Ϣ */
		Renderer2D::BeginScene(camera);

		/* ��������ͼƬʵ�� */
		const auto entity_group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto& entity : entity_group)
		{
			auto [transform_component, sprite_component] = entity_group.get<TransformComponent, SpriteComponent>(entity);
			Renderer2D::DrawSprite(transform_component.GetTransform(), &sprite_component, static_cast<int>(entity));
		}

		/* �ύ�������� */
		Renderer2D::EndScene();
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
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
