#pragma once
#include <Wuya.h>

class TerminatorApp : public Wuya::Application
{
public:
	TerminatorApp();
	~TerminatorApp() override {}
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TerminatorApp();
}
