#pragma once
#include <Wuya.h>

namespace Wuya
{

class TestApp : public Application
{
public:
	TestApp();
	~TestApp() override = default;
};

UniquePtr<Application> CreateApplication();

}
