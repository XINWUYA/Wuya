#pragma once
#include <Helios.h>

class TestLayer : public Helios::ILayer
{
public:
	~TestLayer() override;
	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;
	void OnEvent(Helios::IEvent* event) override;

private:
	Helios::SharedPtr<Helios::VertexArray> m_pVertexArray;
	Helios::UniquePtr<Helios::ShaderLibrary> m_pShaderLibrary;
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
