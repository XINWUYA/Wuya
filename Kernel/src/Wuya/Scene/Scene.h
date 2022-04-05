#pragma once
#include <entt/entt.hpp>

namespace Wuya
{
	class Entity;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity& entity);

		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		template<typename T>
		void OnComponentAdded(Entity& entity, T& component);

	private:
		entt::registry m_Registry;
	};
}
