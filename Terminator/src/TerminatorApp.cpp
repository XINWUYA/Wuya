#include "TerminatorApp.h"
#include "EditorLayer.h"

/* ��������ڵ㣬��Ҫ��֤����Wuya.h֮�� */
#include <Wuya/Application/EntryPoint.h>

TerminatorApp::TerminatorApp() : Application("Terminator")
{
	PushLayer(new EditorLayer());
}
