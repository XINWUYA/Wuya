#include "TestApp.h"
#include "SampleSkyBox.h"

namespace Helios
{
TestApp::TestApp() : Application("Sample_SkyBox")
{
	PushLayer(CreateSharedPtr<SampleSkyBox>());
}

UniquePtr<Application> CreateApplication()
{
	return CreateUniquePtr<TestApp>();
}

}