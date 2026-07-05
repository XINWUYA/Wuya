#pragma once
#include <Kernel.h>

#include "EditorBuiltinCamera.h"

class TestLayer : public Helios::ILayer
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

	Helios::SharedPtr<Helios::VertexArray> m_pVertexArray;
	Helios::UniquePtr<Helios::ShaderLibrary> m_pShaderLibrary;
	Helios::SharedPtr<Helios::Texture2D> m_pTexture2D;
	Helios::UniquePtr<Helios::EditorCamera> m_pEditorCamera;
	Helios::SharedPtr<Helios::UniformBuffer> m_pCameraCBuffer;
	CameraParams m_CameraParams{};
};

class TestApp : public Helios::Application
{
public:
	TestApp();
	~TestApp() override {}
};

Helios::UniquePtr<Helios::Application> Helios::CreateApplication()
{
	return Helios::CreateUniquePtr<TestApp>();
}