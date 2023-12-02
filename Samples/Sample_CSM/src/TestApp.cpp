#include "TestApp.h"
#include "SampleCSM.h"

namespace Wuya
{
TestApp::TestApp() : Application("Sample_CSM")
{
	PushLayer(CreateSharedPtr<SampleCSM>());
}

UniquePtr<Application> CreateApplication()
{
	return CreateUniquePtr<TestApp>();
}

}