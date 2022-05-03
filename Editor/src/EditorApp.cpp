#include "Pch.h"
#include "EditorApp.h"
#include "EditorLayer.h"

namespace Wuya
{
	EditorApp::EditorApp() : Application("Editor")
	{
		PushLayer(CreateSharedPtr<EditorLayer>());
	}
}