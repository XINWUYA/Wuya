#pragma once

namespace Wuya
{
	class EditorApp : public Application
	{
	public:
		EditorApp();
		~EditorApp() {}
	};

	UniquePtr<Application> CreateApplication()
	{
		return CreateUniquePtr<EditorApp>();
	}
}
