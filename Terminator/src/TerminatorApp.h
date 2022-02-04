#pragma once
#include <Wuya.h>

class TerminatorApp : public Wuya::Application
{
public:
	TerminatorApp();
	~TerminatorApp() {}

	virtual void Run() override;

private:
	unsigned int m_VAO, m_VBO, m_Index;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TerminatorApp();
}
