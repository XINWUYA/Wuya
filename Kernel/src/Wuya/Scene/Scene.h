#pragma once
#include <entt/entt.hpp>

namespace Wuya
{
	class Entity;

	/* ������ */
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		/* ����һ��ʵ�� */
		Entity CreateEntity(const std::string& name = std::string());
		/* ����ʵ�� */
		void DestroyEntity(Entity& entity);

		/* ��ȡ������Registry */
		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		/* ����ģʽ���³��� */
		void OnUpdateRuntime(float delta_time);

		/* �༭ģʽ���³��� */
		void OnUpdateEditor(const SharedPtr<class EditorCamera>& camera, float delta_time);

		/* ��ȡ�����ʵ�� */
		Entity GetPrimaryCameraEntity();

		template<typename T>
		void OnComponentAdded(Entity& entity, T& component);

	private:
		/* �����е�����ʵ�嶼��ע�ᵽ���� */
		entt::registry m_Registry;
	};
}
