#include "Pch.h"
#include "EditorApp.h"
#include "ModelEditor.h"

namespace Helios
{
	ModelEditorApp::ModelEditorApp() : Application("ModelEditor")
	{
		PushLayer(CreateSharedPtr<ModelEditor>());
	}
}