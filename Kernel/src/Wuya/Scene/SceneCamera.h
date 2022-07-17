#pragma once
#include "Wuya/Renderer/Camera.h"

namespace Wuya
{
	/* ͶӰ������� */
	struct PerspectiveCameraDesc
	{
		float Fov{ glm::radians(45.0f) };
		float Near{ 0.01f };
		float Far{ 1000.0f };

		PerspectiveCameraDesc() = default;
	};

	/* ����������� */
	struct OrthographicCameraDesc
	{
		float HeightSize{ 10.0f };
		float Near{ -1.0f };
		float Far{ 1.0f };

		OrthographicCameraDesc() = default;
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
		const SharedPtr<PerspectiveCameraDesc>& GetPerspectiveCameraDesc() const { return m_pPerspectiveCameraDesc; }
		const SharedPtr<OrthographicCameraDesc>& GetOrthographicCameraDesc() const { return m_pOrthographicCameraDesc; }
		void SetPerspectiveCameraDesc(const SharedPtr<PerspectiveCameraDesc>& desc);
		void SetPerspectiveCameraDesc(const PerspectiveCameraDesc& desc);
		void SetOrthographicCameraDesc(const SharedPtr<OrthographicCameraDesc>& desc);
		void SetOrthographicCameraDesc(const OrthographicCameraDesc& desc);
		/* ��߱� */
		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float ratio);

	private:
		/* �������ͶӰ���� */
		void UpdateProjectionMatrix();

		/* ͶӰ���� */
		ProjectionType m_ProjectionType{ ProjectionType::Perspective };
		/* ����������� */
		SharedPtr<PerspectiveCameraDesc> m_pPerspectiveCameraDesc{ nullptr };
		SharedPtr<OrthographicCameraDesc> m_pOrthographicCameraDesc{ nullptr };
		/* ��߱� */
		float m_AspectRatio{ 1.778f };
	};
}
