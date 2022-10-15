#include "Pch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Wuya
{
	SceneCamera::SceneCamera(const std::string& name, float aspect_ratio)
		: Camera(name), m_AspectRatio(aspect_ratio)
	{
		PROFILE_FUNCTION();

		m_pPerspectiveCameraDesc = CreateSharedPtr<PerspectiveCameraDesc>();
		m_pOrthographicCameraDesc = CreateSharedPtr<OrthographicCameraDesc>();
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetProjectionType(ProjectionType type)
	{
		m_ProjectionType = type;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetPerspectiveCameraDesc(const SharedPtr<PerspectiveCameraDesc>& desc)
	{
		m_pPerspectiveCameraDesc = desc;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetPerspectiveCameraDesc(const PerspectiveCameraDesc& desc)
	{
		if (!m_pPerspectiveCameraDesc)
			m_pPerspectiveCameraDesc = Wuya::CreateSharedPtr<PerspectiveCameraDesc>();

		m_pPerspectiveCameraDesc->Fov = desc.Fov;
		m_pPerspectiveCameraDesc->Near = desc.Near;
		m_pPerspectiveCameraDesc->Far = desc.Far;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetOrthographicCameraDesc(const SharedPtr<OrthographicCameraDesc>& desc)
	{
		m_pOrthographicCameraDesc = desc;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetOrthographicCameraDesc(const OrthographicCameraDesc& desc)
	{
		if (!m_pOrthographicCameraDesc)
			m_pOrthographicCameraDesc = Wuya::CreateSharedPtr<OrthographicCameraDesc>();

		m_pOrthographicCameraDesc->HeightSize = desc.HeightSize;
		m_pOrthographicCameraDesc->Near = desc.Near;
		m_pOrthographicCameraDesc->Far = desc.Far;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetAspectRatio(float ratio)
	{
		m_AspectRatio = ratio;
		UpdateProjectionMatrix();
	}

	void SceneCamera::UpdateProjectionMatrix()
	{
		PROFILE_FUNCTION();

		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			m_ProjectionMatrix = glm::perspective(
				m_pPerspectiveCameraDesc->Fov,
				m_AspectRatio, 
				m_pPerspectiveCameraDesc->Near,
				m_pPerspectiveCameraDesc->Far
			);
			break;
		case ProjectionType::Orthographic:
			{
				m_ProjectionMatrix = glm::ortho(
					-m_pOrthographicCameraDesc->HeightSize * m_AspectRatio * 0.5f,
					m_pOrthographicCameraDesc->HeightSize * m_AspectRatio * 0.5f,
					-m_pOrthographicCameraDesc->HeightSize * 0.5f,
					m_pOrthographicCameraDesc->HeightSize * 0.5f,
					m_pOrthographicCameraDesc->Near,
					m_pOrthographicCameraDesc->Far
				);
			}
			break;
		}
	}
}
