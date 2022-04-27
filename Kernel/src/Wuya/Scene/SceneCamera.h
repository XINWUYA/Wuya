#pragma once
#include "Wuya/Renderer/Camera.h"

namespace Wuya
{
	/* ͶӰ������� */
	struct SceneCameraDesc_Perspective
	{
		float Fov{ glm::radians(45.0f) };
		float Near{ 0.01f };
		float Far{ 1000.0f };

		SceneCameraDesc_Perspective() = default;
	};

	/* ����������� */
	struct SceneCameraDesc_Orthographic
	{
		float HeightSize{ 10.0f };
		float Near{ -1.0f };
		float Far{ 1.0f };

		SceneCameraDesc_Orthographic() = default;
	};

	/* ��������� */
	class SceneCamera : public Camera
	{
	public:
		/* ͶӰ���� */
		enum class ProjectionType : uint8_t
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera(float aspect_ratio = 1.778f);
		~SceneCamera() override = default;

		/* ͶӰ���� */
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type);
		/* ������� */
		const SharedPtr<SceneCameraDesc_Perspective>& GetSceneCameraDesc_Perspective() const { return m_pCameraDesc_Perspective; }
		const SharedPtr<SceneCameraDesc_Orthographic>& GetSceneCameraDesc_Orthographic() const { return m_pCameraDesc_Orthographic; }
		void SetSceneCameraDesc_Perspective(const SharedPtr<SceneCameraDesc_Perspective>& desc);
		void SetSceneCameraDesc_Perspective(const SceneCameraDesc_Perspective& desc);
		void SetSceneCameraDesc_Orthographic(const SharedPtr<SceneCameraDesc_Orthographic>& desc);
		void SetSceneCameraDesc_Orthographic(const SceneCameraDesc_Orthographic& desc);
		/* ��߱� */
		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float ratio);

	private:
		/* �������ͶӰ���� */
		void UpdateProjectionMatrix();

		/* ͶӰ���� */
		ProjectionType m_ProjectionType{ ProjectionType::Perspective };
		/* ����������� */
		SharedPtr<SceneCameraDesc_Perspective> m_pCameraDesc_Perspective{ nullptr };
		SharedPtr<SceneCameraDesc_Orthographic> m_pCameraDesc_Orthographic{ nullptr };
		/* ��߱� */
		float m_AspectRatio{ 1.778f };
	};
}
