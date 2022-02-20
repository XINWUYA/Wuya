#include "pch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Wuya
{
	OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom)
	{
		SetProjection(left, right, top, bottom);
	}

	OrthographicCamera::~OrthographicCamera()
	{
	}

	void OrthographicCamera::SetProjection(float left, float right, float top, float bottom)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		UpdateCameraMatrix();
	}

	void OrthographicCamera::SetRotation(float rotation)
	{
		m_Rotation = rotation;
		UpdateCameraMatrix();
	}

	void OrthographicCamera::UpdateCameraMatrix()
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
