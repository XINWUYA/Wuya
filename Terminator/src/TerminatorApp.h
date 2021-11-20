#pragma once
#include <Wuya.h>

class TerminatorApp : public Wuya::Application
{
public:
	TerminatorApp();
	~TerminatorApp() {}

	virtual void Run() override;
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TerminatorApp();
}
