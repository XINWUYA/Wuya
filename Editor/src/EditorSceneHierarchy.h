#pragma once
#include <Wuya.h>

namespace Wuya
{
	class Scene;
	class Entity;
}

/* 场景实体列表 */
class EditorSceneHierarchy
{
public:
	EditorSceneHierarchy();
	EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene);
	~EditorSceneHierarchy() = default;

	/* 所属场景 */
	void SetOwnerScene(const Wuya::SharedPtr<Wuya::Scene>& scene);
	/* 绘制相关UI */
	void OnImGuiRender();
	/* 选中实体 */
	const Wuya::Entity& GetSelectedEntity() const { return m_SelectedEntity; }
	void SetSelectedEntity(const Wuya::Entity& entity);

private:
	/* 初始化图标 */
	void InitIcons();

	/* 显示场景实体列表UI */
	void ShowSceneHierarchyUI();
	/* 显示选中实体属性 */
	void ShowEntityPropertiesUI();
	/* 显示实体节点（SceneHierarchy中的节点）*/
	void ShowEntityNode(Wuya::Entity& entity);
	/* 显示实体属性组件 */
	void ShowEntityComponents(Wuya::Entity& entity);

	/* 所属场景 */
	Wuya::SharedPtr<Wuya::Scene> m_pOwnerScene;
	/* 选中实体 */
	Wuya::Entity m_SelectedEntity;

	/* 图标 */
	Wuya::SharedPtr<Wuya::Texture2D> m_pAddComponentIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pMenuIcon;
};

