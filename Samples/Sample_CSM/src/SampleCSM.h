#pragma once
#include <Wuya.h>

namespace Wuya
{

class SampleCamera;

class SampleCSM : public ILayer
{
public:
	~SampleCSM() override = default;
	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;

private:
	SharedPtr<SampleCamera> m_pCamera;
	SharedPtr<Scene> m_pScene;
	SharedPtr<DirectionalLight> m_pDirectionLight;
};

}
