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

		/* ִ��ģʽ */
		enum class PlayMode : uint8_t
		{
			Edit = 0,	/* �༭ģʽ */
			Runtime		/* ����ģʽ */
		};

		SharedPtr<VertexArray> m_pVertexArray;
		SharedPtr<Texture2D> m_pTexture2D;
		UniquePtr<ShaderLibrary> m_pShaderLibrary;
		SharedPtr<class EditorCamera> m_pEditorCamera;
		SharedPtr<OrthographicCameraController> m_pOrthographicCameraController;
		SharedPtr<FrameBuffer> m_pFrameBuffer;

		/* ������ */
		SharedPtr<Scene> m_pMainScene;
		/* ����ʵ������� */
		EditorSceneHierarchy m_SceneHierarchy;
		/* ��Դ������ */
		EditorResourceBrowser m_ResourceBrowser;
		/* ѡ��ʵ�� */
		Entity m_HoveredEntity;
		/* �ӿڳߴ� */
		glm::uvec2 m_ViewportSize{};
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		glm::vec4 m_ViewportRegion{};
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
