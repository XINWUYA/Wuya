#include "TerminatorApp.h"
/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>
#include <Wuya/Events/ApplicationEvent.h>

#include "EditorLayer.h"

TerminatorApp::TerminatorApp() : Application("Terminator")
{
	PushLayer(new EditorLayer());
}

void TerminatorApp::Run()
{
	__super::Run();

	CLIENT_LOG_WARN("Hello! {0}", 5);

	Wuya::WindowResizeEvent e(1280, 720);

	if (e.IsInCategory(Wuya::EventCategoryApplication))
	{
		CLIENT_LOG_DEBUG(e.GetName());
		CLIENT_LOG_CRITICAL(e);
	}
}
