#include "pch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Wuya
{
	SceneCamera::SceneCamera(float aspect_ratio)
		: m_AspectRatio(aspect_ratio)
	{
		PROFILE_FUNCTION();

		m_pCameraDesc_Perspective = CreateSharedPtr<SceneCameraDesc_Perspective>();
		m_pCameraDesc_Orthographic = CreateSharedPtr<SceneCameraDesc_Orthographic>();
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetProjectionType(ProjectionType type)
	{
		m_ProjectionType = type;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetSceneCameraDesc_Perspective(const SharedPtr<SceneCameraDesc_Perspective>& desc)
	{
		m_pCameraDesc_Perspective = desc;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetSceneCameraDesc_Perspective(const SceneCameraDesc_Perspective& desc)
	{
		if (!m_pCameraDesc_Perspective)
			m_pCameraDesc_Perspective = Wuya::CreateSharedPtr<SceneCameraDesc_Perspective>();

		m_pCameraDesc_Perspective->Fov = desc.Fov;
		m_pCameraDesc_Perspective->Near = desc.Near;
		m_pCameraDesc_Perspective->Far = desc.Far;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetSceneCameraDesc_Orthographic(const SharedPtr<SceneCameraDesc_Orthographic>& desc)
	{
		m_pCameraDesc_Orthographic = desc;
		UpdateProjectionMatrix();
	}

	void SceneCamera::SetSceneCameraDesc_Orthographic(const SceneCameraDesc_Orthographic& desc)
	{
		if (!m_pCameraDesc_Orthographic)
			m_pCameraDesc_Orthographic = Wuya::CreateSharedPtr<SceneCameraDesc_Orthographic>();

		m_pCameraDesc_Orthographic->HeightSize = desc.HeightSize;
		m_pCameraDesc_Orthographic->Near = desc.Near;
		m_pCameraDesc_Orthographic->Far = desc.Far;
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
				m_pCameraDesc_Perspective->Fov, 
				m_AspectRatio, 
				m_pCameraDesc_Perspective->Near, 
				m_pCameraDesc_Perspective->Far
			);
			break;
		case ProjectionType::Orthographic:
			{
				m_ProjectionMatrix = glm::ortho(
					-m_pCameraDesc_Orthographic->HeightSize * m_AspectRatio * 0.5f,
					m_pCameraDesc_Orthographic->HeightSize * m_AspectRatio * 0.5f,
					-m_pCameraDesc_Orthographic->HeightSize * 0.5f,
					m_pCameraDesc_Orthographic->HeightSize * 0.5f,
					m_pCameraDesc_Orthographic->Near,
					m_pCameraDesc_Orthographic->Far
				);
			}
			break;
		}
	}
}
