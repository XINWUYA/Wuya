#include "TestApp.h"
#include "SampleIBL.h"

namespace Helios
{
TestApp::TestApp() : Application("Sample_IBL")
{
	PushLayer(CreateSharedPtr<SampleIBLLayer>());
}

UniquePtr<Application> CreateApplication()
{
	return CreateUniquePtr<TestApp>();
}

}