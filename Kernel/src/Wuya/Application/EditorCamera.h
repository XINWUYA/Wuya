#pragma once
#include "Wuya/Renderer/Camera.h"

namespace Wuya
{
	class IEvent;

	enum class EditorCameraMode
	{
		Free = 0, // ����ģʽ
		Focus, // �۽�ģʽ
	};

	// ͶӰ���
	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float fov = 45.0f, float aspect_ratio = 1.778f, float near_clip = 0.1f, float far_clip = 1000.0f);
		~EditorCamera() override;

		void OnUpdate();
		void OnEvent(IEvent* event) {}

		void SetViewportSize(float width, float height);

		void SetDistance(float distance) { m_Distance = distance; }
		float GetDistance() const { return m_Distance; }

		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
		void SetCameraMode(EditorCameraMode mode) { m_Mode = mode; }
		void SetFocalPoint(const glm::vec3& position) { m_FocalPoint = position; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		float GetAspectRatio() const { return m_AspectRatio; }

		const glm::vec3& GetUpDir() const { return m_UpDirection; }
		const glm::vec3& GetRightDir() const { return m_RightDirection; }
		const glm::vec3& GetForwardDir() const { return m_ForwardDirection; }

		glm::quat GetOrientation() const;

	private:
		void UpdateProjectionMatrix();
		void UpdateViewMatrix();
		void UpdateCameraDirections();

		// �۽�ģʽ
		void OnMousePan(const glm::vec2& delta); // ����ƽ��
		void OnMouseRotate(const glm::vec2& delta); // �ƾ۽�������ת
		void OnMouseZoom(float delta); // ��Զ����

		glm::vec2 PanSpeed() const;
		float RotateSpeed() const;
		float ZoomSpeed() const;

		float m_Fov{ 45.0f };
		float m_AspectRatio{ 1.778f };
		float m_NearClip{ 0.1f };
		float m_FarClip{ 1000.0f };

		float m_Pitch{ 0.0f }, m_Yaw{ 0.0f };
		float m_Distance{ 10.0f };
		float m_ViewportWidth{ 1280.0f }, m_ViewportHeight{ 720.0f };

		float m_MoveSpeed{ 1.0f };

		glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_RightDirection{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };

		glm::vec3 m_FocalPoint{ 0.0f, 0.0f, 0.0f };
		glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

		EditorCameraMode m_Mode{ EditorCameraMode::Focus };
	};
}
