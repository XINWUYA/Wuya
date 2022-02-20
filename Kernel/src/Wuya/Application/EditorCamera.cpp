#include "pch.h"
#include "EditorCamera.h"

#include <glm/gtx/quaternion.hpp>

#include "Wuya/Core/Input.h"

namespace Wuya
{
	EditorCamera::EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip)
		: m_Fov(fov), m_AspectRatio(aspect_ratio), m_NearClip(near_clip), m_FarClip(far_clip)
	{
		UpdateCameraDirections();

		UpdateProjectionMatrix();
		UpdateViewMatrix();
	}

	EditorCamera::~EditorCamera()
	{
	}

	void EditorCamera::OnUpdate()
	{
		switch (m_Mode)
		{
		case EditorCameraMode::Free: 
			break;
		case EditorCameraMode::Focus:
		{
			auto mouse_pos = Input::GetMousePos();
			auto delta = (mouse_pos - m_LastMousePosition) * 0.003f;
			m_LastMousePosition = mouse_pos;

			if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
				OnMousePan(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
				OnMouseRotate(delta);
			else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
				OnMouseZoom(delta.y);

			break;
		}
		default: 
			CORE_LOG_ERROR("Wrong camera mode!");
			CORE_ASSERT(false);
			break;
		}

		UpdateViewMatrix();
	}

	void EditorCamera::SetViewportSize(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		UpdateProjectionMatrix();
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void EditorCamera::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void EditorCamera::UpdateViewMatrix()
	{
		switch (m_Mode)
		{
		case EditorCameraMode::Free:
		{
			m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
			m_ViewMatrix = glm::inverse(m_ViewMatrix);
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		}
		case EditorCameraMode::Focus:
		{
			m_Position = m_FocalPoint - GetForwardDir() * m_Distance; // 相机始终围绕聚焦点
			m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
			m_ViewMatrix = glm::inverse(m_ViewMatrix);
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		}
		default:
			CORE_LOG_ERROR("Wrong camera mode!");
			CORE_ASSERT(false);
			break;
		}
	}

	void EditorCamera::UpdateCameraDirections()
	{
		const auto orientation = GetOrientation();
		m_UpDirection = glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
		m_RightDirection = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));
		m_ForwardDirection = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void EditorCamera::OnMousePan(const glm::vec2& delta)
	{
		const auto speed = PanSpeed();
		m_FocalPoint += -GetRightDir() * delta.x * speed.x * m_Distance;
		m_FocalPoint += GetUpDir() * delta.y * speed.y * m_Distance;
	}

	void EditorCamera::OnMouseRotate(const glm::vec2& delta)
	{
		const float yaw_sign = GetUpDir().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yaw_sign * delta.x * RotateSpeed();
		m_Pitch += delta.y * RotateSpeed();

		UpdateCameraDirections();
	}

	void EditorCamera::OnMouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDir();
			m_Distance = 1.0f;
		}
	}

	glm::vec2 EditorCamera::PanSpeed() const
	{
		const float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		const float speed_x = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		const float speed_y = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return glm::vec2(speed_x, speed_y);
	}

	float EditorCamera::RotateSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
}
