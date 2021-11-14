#include "TerminatorApp.h"
#include <Wuya.h>

class TerminatorApp : public Wuya::Application
{
public:
	TerminatorApp() {}
	~TerminatorApp() {}

	virtual void Run() override;
};

Wuya::Application* Wuya::CreateApplication()
{
	return new TerminatorApp();
}

void TerminatorApp::Run()
{
	CLIENT_LOG_WARN("Hello! {0}", 5);
}
