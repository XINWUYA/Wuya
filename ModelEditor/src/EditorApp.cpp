#include "Pch.h"
#include "EditorApp.h"
#include "ModelEditorLayer.h"

namespace Wuya
{
	ModelEditorApp::ModelEditorApp() : Application("ModelEditor")
	{
		PushLayer(CreateSharedPtr<ModelEditorLayer>());
	}
}