#pragma once
#include <Wuya.h>

class EditorApp : public Wuya::Application
{
public:
	EditorApp();
	~EditorApp() override {}
};

Wuya::Application* Wuya::CreateApplication()
{
	return new EditorApp();
}