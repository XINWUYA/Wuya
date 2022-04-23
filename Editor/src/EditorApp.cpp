#include "pch.h"
#include "EditorApp.h"
#include "EditorLayer.h"

/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>

EditorApp::EditorApp() : Application("Editor")
{
	PushLayer(Wuya::CreateSharedPtr<EditorLayer>());
}
