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
	//Wuya::Logger::GetClientLogger()->info("Hello!");
}

extern Wuya::Application* Wuya::CreateApplication();

int main(int argc, char** argv)
{
	Wuya::Logger::Init();
	CORE_LOG("Wuya 1.0.0");
	CLIENT_LOG_WARN("Hello! {0}", 5);

	auto app = Wuya::CreateApplication();
	app->Run();

	return 0;
}