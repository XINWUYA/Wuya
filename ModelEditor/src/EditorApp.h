#pragma once

namespace Helios
{
	class ModelEditorApp : public Application
	{
	public:
		ModelEditorApp();
		~ModelEditorApp() {}
	};

	UniquePtr<Application> CreateApplication()
	{
		return CreateUniquePtr<ModelEditorApp>();
	}
}
