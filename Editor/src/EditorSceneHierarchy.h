#pragma once
#include <Wuya.h>

namespace Wuya
{
	class Scene;
	class Entity;
}

/* ����ʵ���б� */
class EditorSceneHierarchy
{
public:
	EditorSceneHierarchy();
	EditorSceneHierarchy(const Wuya::SharedPtr<Wuya::Scene>& scene);
	~EditorSceneHierarchy() = default;

	/* �������� */
	void SetOwnerScene(const Wuya::SharedPtr<Wuya::Scene>& scene);
	/* �������UI */
	void OnImGuiRender();
	/* ѡ��ʵ�� */
	const Wuya::Entity& GetSelectedEntity() const { return m_SelectedEntity; }
	void SetSelectedEntity(const Wuya::Entity& entity);

private:
	/* ��ʼ��ͼ�� */
	void InitIcons();

	/* ��ʾ����ʵ���б�UI */
	void ShowSceneHierarchyUI();
	/* ��ʾѡ��ʵ������ */
	void ShowEntityPropertiesUI();
	/* ��ʾʵ��ڵ㣨SceneHierarchy�еĽڵ㣩*/
	void ShowEntityNode(Wuya::Entity& entity);
	/* ��ʾʵ��������� */
	void ShowEntityComponents(Wuya::Entity& entity);

	/* �������� */
	Wuya::SharedPtr<Wuya::Scene> m_pOwnerScene;
	/* ѡ��ʵ�� */
	Wuya::Entity m_SelectedEntity;

	/* ͼ�� */
	Wuya::SharedPtr<Wuya::Texture2D> m_pAddComponentIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pMenuIcon;
};

