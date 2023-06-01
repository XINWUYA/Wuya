#pragma once
#include "SceneHierarchy.h"
#include "EditorResourceBrowser.h"
#include "EditorBuiltinCamera.h"
#include "EditorCommon.h"

namespace Wuya
{
	/* Editor���� */
	class SceneEditorLayer final : public ILayer
	{
	public:
		SceneEditorLayer();
		~SceneEditorLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

		/* �½����� */
		void NewScene();
		/* ���볡����ͨ�������ҵ�ָ�������ļ����� */
		void ImportScene();
		/* ���泡�� */
		void SaveScene();
		/* ���泡����ָ��·�� */
		void SaveSceneAs();

		void Active(bool active = true) { m_IsActivated = active; }
		bool IsActivated() const { return m_IsActivated; }

		void SetGizmoType(int type) { m_GizmoType = type; }
		void SetPlayMode(PlayMode mode) { m_PlayMode = mode; }

	private:
		/* �����ӿ� */
		void UpdateViewport();
		/* ��Ӧ���� */
		bool OnKeyPressed(class KeyPressedEvent* event);
		/* ��Ӧ��� */
		bool OnMouseButtonPressed(class MouseButtonPressedEvent* event);
		/* �л�����ģʽ */
		void OnPlayModeChanged();
		/* ��Ӧ��ק�ļ��������� */
		void OnDragItemToScene(const std::filesystem::path& path);

		/* ��ʾ�������ӿ� */
		void ShowSceneViewportUI();
		/* ��ʾ��Ⱦͳ����Ϣ */
		void ShowStatisticInfoUI();
		/* ѡ��Entityʱ��ʾ����Gizmo */
		void ShowOperationGizmoUI();

		/* ���ѡ��Entityʱ����Ӧ */
		void CheckMouseSelectEntity();

		/* �༭����� */
		UniquePtr<EditorCamera> m_pEditorCamera{ nullptr };

		/* ������ */
		SharedPtr<Scene> m_pMainScene;
		/* ��ǰ����·�� */
		std::string m_ActiveScenePath{};
		/* ����ʵ������� */
		SceneHierarchy m_SceneHierarchy;
		/* ѡ��ʵ�� */
		Entity m_HoveredEntity;
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};
		/* �ӿڴ��ڱ����� */
		bool m_IsViewportFocused{ false };
		/* ���ͣ�����ӿ��� */
		bool m_IsViewportHovered{ false };
		/* �ƶ�����ת������UI */
		int m_GizmoType = -1;
		/* Ĭ��Ϊ�༭ģʽ */
		PlayMode m_PlayMode{ PlayMode::Edit };
		/* ��ǰ�༭���Ƿ����� */
		bool m_IsActivated{ true };
	};
}
