#include "Pch.h"
#include "Entity.h"
#include "Scene.h"

namespace Wuya
{
	Entity::Entity(entt::entity handle, const SharedPtr<Scene>& owner_scene)
		: m_EntityHandle(handle), m_OwnerScene(owner_scene)
	{
	}

	bool Entity::operator==(const Entity& other) const
	{
		return m_EntityHandle == other.m_EntityHandle && m_OwnerScene.lock() == other.m_OwnerScene.lock();
	}

	bool Entity::operator!=(const Entity& other) const
	{
		return !(*this == other);
	}

# if 0
	template <typename T, typename ... Args>
	T& Entity::AddComponent(Args&&... args)
	{
		ASSERT(!HasComponent<T>(), "Component already existed!");

		T& component = m_OwnerScene->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		m_OwnerScene->OnComponentAdded<T>(*this, component);

		return component;
	}

	template <typename T>
	void Entity::RemoveComponent()
	{
		ASSERT(!HasComponent<T>(), "Entity doesn't have component!");

		m_OwnerScene->GetRegistry().remove<T>(m_EntityHandle);
	}

	template <typename T>
	T& Entity::GetComponent()
	{
		ASSERT(!HasComponent<T>(), "Entity doesn't have component!");

		return m_OwnerScene->GetRegistry().get<T>(m_EntityHandle);
	}

	template <typename T>
	bool Entity::HasComponent() const
	{
		return m_OwnerScene->GetRegistry().all_of<T>(m_EntityHandle);
	}
#endif
}
