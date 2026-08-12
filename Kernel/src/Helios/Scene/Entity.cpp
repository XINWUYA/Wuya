#include "Pch.h"
#include "Entity.h"
#include "Scene.h"

namespace Helios
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
}
