#pragma once
#include <entt/entt.hpp>

namespace tinyxml2
{
	class XMLElement;
}

namespace Wuya
{
	class Entity;

	/* 场景类 */
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		/* 创建一个实体 */
		Entity CreateEntity(const std::string& name = std::string());
		/* 销毁实体 */
		void DestroyEntity(Entity& entity);

		/* 获取场景的Registry */
		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		/* 运行模式更新场景 */
		void OnUpdateRuntime(float delta_time);

		/* 编辑模式更新场景 */
		void OnUpdateEditor(const SharedPtr<class EditorCamera>& camera, float delta_time);

		/* 获取主相机实体 */
		Entity GetPrimaryCameraEntity();

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
	};
}
