#pragma once
#include "EditorCommon.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	/* Editor���� */
	class MainEditorLayer final : public ILayer
	{
	public:
		MainEditorLayer();
		~MainEditorLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

	private:
		/* ��Ӧ���� */
		bool OnKeyPressed(class KeyPressedEvent* event);
		/* ��Ӧ��� */
		bool OnMouseButtonPressed(class MouseButtonPressedEvent* event);

		/* �½����� */
		void NewScene();
		/* ���볡����ͨ�������ҵ�ָ�������ļ����� */
		void ImportScene();
		/* ���泡�� */
		void SaveScene();
		/* ���泡����ָ��·�� */
		void SaveSceneAs();

		/* ��ʾ�˵���UI */
		void ShowMenuUI();
		/* ��ʾ��������UI */
		void ShowSceneControllerUI();
		/* ��ʾ��Ⱦͳ����Ϣ */
		void ShowStatisticInfoUI();

		/* ֪ͨ���������� */
		void NotifyGizmoTypeChanged();
		void NotifyPlayModeChanged();

		/* ��Դ������ */
		EditorResourceBrowser m_ResourceBrowser;
		/* Ĭ��Ϊ�༭ģʽ */
		PlayMode m_PlayMode{ PlayMode::Edit };
		/* �ƶ�����ת������UI */
		int m_GizmoType = -1;

		/* ���ڿ��� */
		bool m_ActiveSceneEditor{ false };
		bool m_ActiveModelEditor{ false };
	};
}
