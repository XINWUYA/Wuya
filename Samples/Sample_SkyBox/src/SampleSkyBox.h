#pragma once
#include <Helios.h>

namespace Helios
{

class SampleCameraController;

class SampleSkyBox : public ILayer
{
public:
	~SampleSkyBox() override = default;
	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;

private:
	SharedPtr<Scene> m_pScene;
	SharedPtr<SampleCameraController> m_pCameraController;
};

}
