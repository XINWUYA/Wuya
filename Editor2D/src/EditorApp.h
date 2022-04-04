#pragma once
#include <Wuya.h>

class EditorApp : public Wuya::Application
{
public:
	EditorApp();
	~EditorApp() override {}
};

Wuya::UniquePtr<Wuya::Application> Wuya::CreateApplication()
{
	return CreateUniquePtr<EditorApp>();
}
