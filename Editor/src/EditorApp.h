#pragma once
#include <Wuya.h>

class EditorApp : public Wuya::Application
{
public:
	EditorApp();
	~EditorApp() {}
};

Wuya::UniquePtr<Wuya::Application> Wuya::CreateApplication()
{
	return Wuya::CreateUniquePtr<EditorApp>();
}
