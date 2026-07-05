#include "TestApp.h"
#include "SampleCSM.h"

namespace Helios
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