#pragma once
#include <Wuya/Core/Layer.h>
#include <Wuya.h>

class EditorLayer : public Wuya::ILayer
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
	void UpdateViewport();

	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
	Wuya::UniquePtr<Wuya::EditorCamera> m_pEditorCamera;
	Wuya::UniquePtr<Wuya::OrthographicCameraController> m_pOrthographicCameraController;
	Wuya::SharedPtr<Wuya::FrameBuffer> m_pFrameBuffer;

	glm::uvec2 m_ViewportSize{};
	bool m_IsViewportFocused{ false };
	bool m_IsViewportHovered{ false };
};

