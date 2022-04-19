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

	void NewScene();
	void OpenScene();

private:

	/* 运行模式 */
	enum class PlayMode : uint8_t
	{
		Edit = 0,	/* 编辑模式 */
		Runtime		/* 运行模式 */
	};

	void UpdateViewport();
	bool OnKeyPressed(Wuya::KeyPressedEvent* event);
	bool OnMouseButtonPressed(Wuya::MouseButtonPressedEvent* event);

	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
	Wuya::SharedPtr<Wuya::EditorCamera> m_pEditorCamera;
	Wuya::SharedPtr<Wuya::OrthographicCameraController> m_pOrthographicCameraController;
	Wuya::SharedPtr<Wuya::FrameBuffer> m_pFrameBuffer;

	Wuya::SharedPtr<Wuya::Scene> m_pMainScene;
	EditorSceneHierarchy m_SceneHierarchy;
	EditorResourceBrowser m_ResourceBrowser;
	Wuya::Entity m_HoveredEntity;
	glm::uvec2 m_ViewportSize{};
	bool m_IsViewportFocused{ false };
	bool m_IsViewportHovered{ false };
	PlayMode m_PlayMode{ PlayMode::Edit }; /* 默认为编辑模式 */
};

