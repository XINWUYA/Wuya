#include "Pch.h"
#include "EditorApp.h"
#include "ModelEditor.h"

namespace Wuya
{
	ModelEditorApp::ModelEditorApp() : Application("ModelEditor")
	{
		PushLayer(CreateSharedPtr<ModelEditor>());
	}
}