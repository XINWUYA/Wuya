#pragma once
#include <Helios.h>

namespace Helios
{
	class SampleCameraController;

	class SampleIBLLayer final : public ILayer
	{
	public:
		SampleIBLLayer();
		~SampleIBLLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;

	private:
		SharedPtr<Scene> m_pScene{ nullptr };
		SharedPtr<DirectionalLight> m_pDirectionLight{ nullptr };
		SharedPtr<SampleCameraController> m_pCameraController{ nullptr };

		SharedPtr<Material> m_pPBRMaterial{ nullptr };
		SharedPtr<Material> m_pSkyboxMaterial{ nullptr };
		SharedPtr<DeviceTexture> m_pSkyTexture{ nullptr };

		ReflectionProbeComponent* m_pReflectionProbeComponent{ nullptr };
	};
}
