#include "pch.h"
#include "EditorApp.h"
#include "EditorLayer.h"

/* ��������ڵ㣬��Ҫ��֤����Wuya.h֮�� */
#include <Wuya/Application/EntryPoint.h>

EditorApp::EditorApp() : Application("Editor")
{
	PushLayer(Wuya::CreateSharedPtr<EditorLayer>());
}
