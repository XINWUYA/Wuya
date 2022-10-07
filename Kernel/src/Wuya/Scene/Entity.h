#pragma once
#include <entt/entt.hpp>
#include "Scene.h"

namespace Wuya
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, const SharedPtr<Scene>& owner_scene);
		~Entity() = default;

		/* 添加一个组件 */
		template<typename T, typename ... Args>
		T& AddComponent(Args&&... args)
		{
			if (HasComponent<T>())
			{
				CORE_LOG_ERROR("Component already existed!");
				return GetComponent<T>();
			}

			T& component = m_OwnerScene.lock()->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_OwnerScene.lock()->OnComponentAdded<T>(*this, component);

			return component;
		}

		/* 移除一个组件 */
		template<typename T>
		void RemoveComponent()
		{
			if (!HasComponent<T>())
			{
				CORE_LOG_ERROR("Entity doesn't have component!");
				return;
			}

			m_OwnerScene.lock()->GetRegistry().remove<T>(m_EntityHandle);
		}

		/* 获取组件 */
		template<typename T>
		T& GetComponent()
		{
			ASSERT(HasComponent<T>(), "Entity doesn't have component!");

			return m_OwnerScene.lock()->GetRegistry().get<T>(m_EntityHandle);
		}

		/* 是否具有组件 */
		template<typename T>
		bool HasComponent() const
		{
			/*if (!m_OwnerScene->GetRegistry().valid(m_EntityHandle))
				return false;*/
			return m_OwnerScene.lock()->GetRegistry().all_of<T>(m_EntityHandle);
		}

		/* 重载 */
		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }
		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;

	private:
		/* Entity索引 */
		entt::entity m_EntityHandle{ entt::null };
		/* Entity所属场景 */
		WeakPtr<Scene> m_OwnerScene;
	};
}
