#include "Pch.h"
#include "EditorApp.h"
#include "MainEditor/MainEditorLayer.h"
#include "SceneEditor/SceneEditorLayer.h"
#include "ModelEditor/ModelEditorLayer.h"

namespace Wuya
{
	EditorApp::EditorApp() : Application("Editor")
	{
		PushLayer(CreateSharedPtr<MainEditorLayer>()); /* Ҫ����� */
		PushLayer(CreateSharedPtr<SceneEditorLayer>());
		PushLayer(CreateSharedPtr<ModelEditorLayer>());
	}
}
