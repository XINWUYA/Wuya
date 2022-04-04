#pragma once
#include <Wuya.h>

class TestLayer : public Wuya::ILayer
{
public:
	~TestLayer() override;
	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;

private:
	struct CameraParams
	{
		glm::mat4 ViewProjection;
	};

	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::EditorCamera> m_pEditorCamera;
	Wuya::SharedPtr<Wuya::UniformBuffer> m_pCameraCBuffer;
	CameraParams m_CameraParams{};
};

class TestApp : public Wuya::Application
{
public:
	TestApp();
	~TestApp() override {}
};

Wuya::UniquePtr<Wuya::Application> Wuya::CreateApplication()
{
	return Wuya::CreateUniquePtr<TestApp>();
}