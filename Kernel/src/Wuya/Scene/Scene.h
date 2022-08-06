#pragma once
#include <entt/entt.hpp>

namespace tinyxml2
{
	class XMLElement;
}

namespace Wuya
{
	class Entity;
	class RenderView;
	class Texture;

	/* ������ */
	class Scene final : public std::enable_shared_from_this<Scene>
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
		void OnUpdateEditor(const SharedPtr<class Camera>& camera, float delta_time);

		/* ��ȡ�����ʵ�� */
		Entity GetPrimaryCameraEntity();
		/* ��ȡ�������RenderTarget Texture */
		const SharedPtr<Texture>& GetPrimaryCameraRenderTargetTexture() const;

		/* ���л����� */
		void Serializer(const std::string& path);
		bool Deserializer(const std::string& path);

		template<typename T>
		void OnComponentAdded(Entity& entity, T& component);

	private:
		/* ���л�һ��ʵ�� */
		void SerializeEntity(tinyxml2::XMLElement* root_node, Entity& entity);

		/* �����е�����ʵ�嶼��ע�ᵽ���� */
		entt::registry m_Registry;

		/* RenderView�б� */
		std::vector<SharedPtr<RenderView>> m_RenderViews;
	};
}
