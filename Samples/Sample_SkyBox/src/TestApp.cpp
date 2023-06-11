#include "TestApp.h"
#include "SampleSkyBox.h"

namespace Wuya
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