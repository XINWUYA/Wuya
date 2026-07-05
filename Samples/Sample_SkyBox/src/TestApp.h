#pragma once
#include <Helios.h>

namespace Helios
{

class TestApp : public Application
{
public:
	TestApp();
	~TestApp() override = default;
};

UniquePtr<Application> CreateApplication();

}
