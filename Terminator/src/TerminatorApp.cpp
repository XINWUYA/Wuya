#include "TerminatorApp.h"
#include "EditorLayer.h"

/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>

TerminatorApp::TerminatorApp() : Application("Terminator")
{
	PushLayer(new EditorLayer());
}
