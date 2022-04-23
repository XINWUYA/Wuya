#pragma once
#include <Wuya/Core/Layer.h>
#include <Wuya.h>
#include "EditorSceneHierarchy.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	class KeyPressedEvent;
	class MouseButtonPressedEvent;
}

class EditorLayer final : public Wuya::ILayer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;
	void OnEvent(Wuya::IEvent* event) override;

private:
	/* �����ӿ� */
	void UpdateViewport();
	/* ��Ӧ���� */
	bool OnKeyPressed(Wuya::KeyPressedEvent* event);
	/* ��Ӧ��� */
	bool OnMouseButtonPressed(Wuya::MouseButtonPressedEvent* event);
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

	/* ִ��ģʽ */
	enum class PlayMode : uint8_t
	{
		Edit = 0,	/* �༭ģʽ */
		Runtime		/* ����ģʽ */
	};

	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
	Wuya::SharedPtr<Wuya::EditorCamera> m_pEditorCamera;
	Wuya::SharedPtr<Wuya::OrthographicCameraController> m_pOrthographicCameraController;
	Wuya::SharedPtr<Wuya::FrameBuffer> m_pFrameBuffer;

	/* ������ */
	Wuya::SharedPtr<Wuya::Scene> m_pMainScene;
	/* ����ʵ������� */
	EditorSceneHierarchy m_SceneHierarchy;
	/* ��Դ������ */
	EditorResourceBrowser m_ResourceBrowser;
	/* */
	Wuya::Entity m_HoveredEntity;
	/* �ӿڳߴ� */
	glm::uvec2 m_ViewportSize{};
	/* �ӿڴ��ڱ����� */
	bool m_IsViewportFocused{ false };
	/* ���ͣ�����ӿ��� */
	bool m_IsViewportHovered{ false };
	/* Ĭ��Ϊ�༭ģʽ */
	PlayMode m_PlayMode{ PlayMode::Edit };
	/* �ƶ�����ת������UI */
	int m_GizmoType = -1;
};

