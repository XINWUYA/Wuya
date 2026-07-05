#include "Pch.h"
#include "Camera.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace Helios
{
	Camera::Camera(CameraProjectionType type, const std::string& name, float fov, float aspect_ratio, float near, float far)
		: m_ProjectionType(type), m_DebugName(name), m_Fov(fov), m_AspectRatio(aspect_ratio), m_NearClip(near), m_FarClip(far)
	{
		PROFILE_FUNCTION();

		m_pRenderView = CreateUniquePtr<RenderView>(m_DebugName + "_RenderView", this);
		m_IsDirty = true;
	}

	/* 更新时间戳 */
	void Camera::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();

		UpdateViewMatrix();
		UpdateProjectionMatrix();
		ConstructRenderView();
	}

	void Camera::SetProjectionType(CameraProjectionType type)
	{
		m_ProjectionType = type;
		m_IsDirty = true;
	}

	/* 根据世界变换矩阵更新相机位置、朝向和视图矩阵 */
	void Camera::SetTransform(const glm::mat4& transform)
	{
		PROFILE_FUNCTION();

		m_Position = glm::vec3(transform[3]);

		const glm::mat3 rotation = glm::mat3(transform);
		m_ForwardDirection = glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		m_UpDirection = glm::normalize(rotation * glm::vec3(0.0f, 1.0f, 0.0f));
		m_RightDirection = glm::normalize(glm::cross(m_ForwardDirection, m_UpDirection));

		/* 恢复正交相机常用的 Z 轴旋转（角度制） */
		m_Rotation = glm::degrees(std::atan2(m_ForwardDirection.y, m_ForwardDirection.x));

		if (m_IsFocus)
		{
			m_FocalPoint = m_Position + m_ForwardDirection * m_FocalDistance;
		}

		m_IsDirty = true;
	}

	/* 设置聚焦模式 */
	void Camera::SetFocus(bool focus)
	{
		if (m_IsFocus == focus)
			return;

		m_IsFocus = focus;

		if (m_IsFocus)
		{
			m_FocalPoint = m_Position + m_ForwardDirection * m_FocalDistance;
		}
		else
		{
			m_Position = m_FocalPoint - m_ForwardDirection * m_FocalDistance;
		}

		m_IsDirty = true;
	}

	/* 设置相机位置 */
	void Camera::SetPosition(const glm::vec3& position)
	{
		if (m_IsFocus)
		{
			m_FocalPoint = position + m_ForwardDirection * m_FocalDistance;
		}
		else
		{
			m_Position = position;
		}

		m_IsDirty = true;
	}

	/* 设置正交相机绕 Z 轴旋转（角度制） */
	void Camera::SetRotation(float rotation)
	{
		m_Rotation = rotation;

		const glm::mat4 rotation_mat = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		m_ForwardDirection = glm::normalize(glm::vec3(rotation_mat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
		m_UpDirection = glm::normalize(glm::vec3(rotation_mat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
		m_RightDirection = glm::normalize(glm::cross(m_ForwardDirection, m_UpDirection));
		m_IsDirty = true;
	}

	/* 更新视图矩阵 */
	void Camera::UpdateViewMatrix()
	{
		PROFILE_FUNCTION();

		if (m_IsFocus)
		{
			m_Position = m_FocalPoint - m_ForwardDirection * m_FocalDistance;
			m_ViewMatrix = glm::lookAt(m_Position, m_FocalPoint, m_UpDirection);
		}
		else
		{
			m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ForwardDirection, m_UpDirection);
		}
	}



	void Camera::UpdateProjectionMatrix()
	{
		PROFILE_FUNCTION();

		if (!m_IsDirty)
			return;

		switch (m_ProjectionType)
		{
		case CameraProjectionType::Perspective:
			m_ProjectionMatrix = glm::perspective(
				m_Fov,
				m_AspectRatio,
				m_NearClip,
				m_FarClip
			);
			break;
		case CameraProjectionType::Orthographic:
			m_ProjectionMatrix = glm::ortho(
				-m_HeightSize * m_AspectRatio * 0.5f,
				m_HeightSize * m_AspectRatio * 0.5f,
				-m_HeightSize * 0.5f,
				m_HeightSize * 0.5f,
				m_NearClip,
				m_FarClip
			);
			break;
		}

		m_IsDirty = false;
	}
}
