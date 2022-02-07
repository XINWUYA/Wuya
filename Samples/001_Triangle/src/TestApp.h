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
	void OnEvent(Wuya::IEvent* event) override;

private:
	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
};

class TestApp : public Wuya::Application
{
public:
	TestApp();
	~TestApp() override {}
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TestApp();
}
