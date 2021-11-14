#pragma once

extern Wuya::Application* Wuya::CreateApplication();

int main(int argc, char** argv)
{
	Wuya::Logger::Init();
	CORE_LOG("Wuya 1.0.0");

	auto app = Wuya::CreateApplication();
	app->Run();

	return 0;
}
