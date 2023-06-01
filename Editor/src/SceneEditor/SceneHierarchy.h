#pragma once

namespace Wuya
{
	class Scene;
	class Entity;

	/* 场景实体列表 */
	class SceneHierarchy
	{
	public:
		SceneHierarchy();
		SceneHierarchy(const SharedPtr<Scene>& scene);
		~SceneHierarchy() = default;

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
		void ShowEntityComponents();

		/* 实体名称组件 */
		void ShowNameComponent();
		/* 增加组件按钮 */
		void ShowAddComponentButton();
		/* 空间变换组件 */
		void ShowTransformComponent();
		/* 图片精灵组件 */
		void ShowSpriteComponent();
		/* 场景相机组件 */
		void ShowCameraComponent();
		/* 模型组件 */
		void ShowModelComponent();
		/* 光源组件 */
		void ShowLightComponent();

		/* 所属场景 */
		SharedPtr<Scene> m_pOwnerScene;
		/* 选中实体 */
		Entity m_SelectedEntity;

		/* 图标 */
		SharedPtr<Texture> m_pAddComponentIcon;
		SharedPtr<Texture> m_pMenuIcon;
	};
}
