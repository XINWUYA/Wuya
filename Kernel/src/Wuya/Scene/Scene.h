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
	class Camera;

	/* 场景类 */
	class Scene final : public std::enable_shared_from_this<Scene>
	{
	public:
		Scene() = default;
		~Scene();

		/* 创建一个实体 */
		Entity CreateEntity(const std::string& name = std::string());
		/* 销毁实体 */
		void DestroyEntity(Entity& entity);
		/* 清空所有实体 */
		void ClearAllEntities() { m_Registry.clear(); }
		/* 销毁指定类型的实体 */
		template <typename T>
		void DestroyTargetEntities() { m_Registry.clear<T>(); }

		/* 获取场景的Registry */
		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		/* 运行模式更新场景 */
		void OnUpdateRuntime(float delta_time);

		/* 编辑模式更新场景 */
		void OnUpdateEditor(Camera* camera, float delta_time);

		/* 获取主相机实体 */
		Entity GetPrimaryCameraEntity();
		/* 获取主相机的RenderTarget Texture */
		const SharedPtr<Texture>& GetPrimaryCameraRenderTargetTexture() const;

		/* 序列化场景 */
		void Serializer(const std::string& path);
		bool Deserializer(const std::string& path);

		template<typename T>
		void OnComponentAdded(Entity& entity, T& component);

	private:
		/* 序列化一个实体 */
		void SerializeEntity(tinyxml2::XMLElement* root_node, Entity& entity);

		/* 场景中的所有实体都将注册到这里 */
		entt::registry m_Registry;

		/* RenderView列表 */
		std::vector<RenderView*> m_RenderViews;
	};
}
