#pragma once
#include <Wuya.h>

namespace Wuya
{
	class Scene;
	class Entity;
}

class EditorSceneHierarchy
{
public:
	EditorSceneHierarchy() = default;
	EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene);
	~EditorSceneHierarchy() = default;

	void SetOwnerScene(const Wuya::SharedPtr<Wuya::Scene>& scene);
	void OnImGuiRender();

	Wuya::Entity GetSelectedEntity() const { return m_SelectedEntity; }
	void SetSelectedEntity(const Wuya::Entity& entity);

private:
	void ShowEntityNode(Wuya::Entity& entity);
	void ShowEntityComponents(Wuya::Entity& entity);

	Wuya::SharedPtr<Wuya::Scene> m_pOwnerScene;
	Wuya::Entity m_SelectedEntity;
};

