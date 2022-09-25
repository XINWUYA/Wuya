#pragma once
#include <Kernel.h>

namespace Wuya
{
	class Scene;
	class Entity;

	/* 场景实体列表 */
	class EditorSceneHierarchy
	{
	public:
		EditorSceneHierarchy();
		EditorSceneHierarchy(const SharedPtr<Scene>& scene);
		~EditorSceneHierarchy() = default;

		/* 所属场景 */
		void SetOwnerScene(const SharedPtr<Scene>& scene);
		/* 绘制相关UI */
		void OnImGuiRender();
		/* 选中实体 */
		const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(const Entity& entity);

	private:
		/* 初始化图标 */
		void InitIcons();

		/* 显示场景实体列表UI */
		void ShowSceneHierarchyUI();
		/* 显示选中实体属性 */
		void ShowEntityPropertiesUI();
		/* 显示实体节点（SceneHierarchy中的节点）*/
		void ShowEntityNode(Entity& entity);
		/* 显示实体属性组件 */
		void ShowEntityComponents(Entity& entity);

		/* 所属场景 */
		SharedPtr<Scene> m_pOwnerScene;
		/* 选中实体 */
		Entity m_SelectedEntity;

		/* 图标 */
		SharedPtr<Texture> m_pAddComponentIcon;
		SharedPtr<Texture> m_pMenuIcon;
	};
}
