#pragma once

namespace Wuya
{
	class Scene;
	class Entity;

	/* ����ʵ���б� */
	class SceneHierarchy
	{
	public:
		SceneHierarchy();
		SceneHierarchy(const SharedPtr<Scene>& scene);
		~SceneHierarchy() = default;

		/* �������� */
		void SetOwnerScene(const SharedPtr<Scene>& scene);
		/* �������UI */
		void OnImGuiRender();
		/* ѡ��ʵ�� */
		const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(const Entity& entity);

	private:
		/* ��ʼ��ͼ�� */
		void InitIcons();

		/* ��ʾ����ʵ���б�UI */
		void ShowSceneHierarchyUI();
		/* ��ʾѡ��ʵ������ */
		void ShowEntityPropertiesUI();
		/* ��ʾʵ��ڵ㣨SceneHierarchy�еĽڵ㣩*/
		void ShowEntityNode(Entity& entity);
		/* ��ʾʵ��������� */
		void ShowEntityComponents();

		/* ʵ��������� */
		void ShowNameComponent();
		/* ���������ť */
		void ShowAddComponentButton();
		/* �ռ�任��� */
		void ShowTransformComponent();
		/* ͼƬ������� */
		void ShowSpriteComponent();
		/* ���������� */
		void ShowCameraComponent();
		/* ģ����� */
		void ShowModelComponent();
		/* ��Դ��� */
		void ShowLightComponent();

		/* �������� */
		SharedPtr<Scene> m_pOwnerScene;
		/* ѡ��ʵ�� */
		Entity m_SelectedEntity;

		/* ͼ�� */
		SharedPtr<Texture> m_pAddComponentIcon;
		SharedPtr<Texture> m_pMenuIcon;
	};
}
