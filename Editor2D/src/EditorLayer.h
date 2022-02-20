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
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::OrthographicCameraController> m_pCameraController;
};

