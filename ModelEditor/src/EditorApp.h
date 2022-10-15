#pragma once
#include <Kernel.h>

namespace Wuya
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
