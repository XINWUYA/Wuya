#pragma once
#include <Wuya.h>

class TerminatorApp : public Wuya::Application
{
public:
	TerminatorApp();
	~TerminatorApp() {}

	virtual void Run() override;

private:
	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TerminatorApp();
}
