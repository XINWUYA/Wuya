#pragma once
#include <Kernel.h>
#include "EditorSceneHierarchy.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	/* Editor���� */
	class EditorLayer final : public ILayer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

	private:
		/* �����ӿ� */
		void UpdateViewport();
		/* ��Ӧ���� */
		bool OnKeyPressed(class KeyPressedEvent* event);
		/* ��Ӧ��� */
		bool OnMouseButtonPressed(class MouseButtonPressedEvent* event);
		/* �л�����ģʽ */
		void OnPlayModeChanged();

		/* �½����� */
		void NewScene();
		/* �򿪳�����ͨ�������ҵ�ָ�������ļ����� */
		void OpenScene();
		/* ���泡�� */
		void SaveScene();
		/* ��ָ������ */
		void OpenScene(const std::filesystem::path& path);

		/* ��ʾ�˵���UI */
		void ShowMenuUI();
		/* ��ʾ��������UI */
		void ShowSceneControllerUI();
		/* ��ʾ�������ӿ� */
		void ShowSceneViewportUI();
		/* ��ʾ��Ⱦͳ����Ϣ */
		void ShowStatisticInfoUI();
		/* ѡ��Entityʱ��ʾ����Gizmo */
		void ShowOperationGizmoUI();

		/* ���ѡ��Entityʱ����Ӧ */
		void CheckMouseSelectEntity();

		/* ִ��ģʽ */
		enum class PlayMode : uint8_t
		{
			Edit = 0,	/* �༭ģʽ */
			Runtime		/* ����ģʽ */
		};

		UniquePtr<ShaderLibrary> m_pShaderLibrary;
		SharedPtr<class EditorCamera> m_pEditorCamera;
		SharedPtr<OrthographicCameraController> m_pOrthographicCameraController;

		/* ������ */
		SharedPtr<Scene> m_pMainScene;
		/* ����ʵ������� */
		EditorSceneHierarchy m_SceneHierarchy;
		/* ��Դ������ */
		EditorResourceBrowser m_ResourceBrowser;
		/* ѡ��ʵ�� */
		Entity m_HoveredEntity;
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};
		/* �ӿڴ��ڱ����� */
		bool m_IsViewportFocused{ false };
		/* ���ͣ�����ӿ��� */
		bool m_IsViewportHovered{ false };
		/* Ĭ��Ϊ�༭ģʽ */
		PlayMode m_PlayMode{ PlayMode::Edit };
		/* �ƶ�����ת������UI */
		int m_GizmoType = -1;
	};
}
