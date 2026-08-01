#pragma once
#include <Helios.h>

namespace Helios
{

class SampleCameraController;

class SampleCSM : public ILayer
{
public:
	~SampleCSM() override = default;
	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;

private:
	SharedPtr<Scene> m_pScene;
	SharedPtr<DirectionalLight> m_pDirectionLight;
	SharedPtr<SampleCameraController> m_pCameraController;
};

}
