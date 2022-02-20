#pragma once

#include <glm/glm.hpp>

namespace Wuya
{
	class IEvent;
	class MouseScrolledEvent;
	class WindowResizeEvent;
	class OrthographicCamera;

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspect_ratio, bool rotatable = false);
		~OrthographicCameraController() = default;

		void OnUpdate(float delta_time);
		void OnEvent(IEvent* event);
		void OnResize(float width, float height);

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() const { return m_ZoomLevel; }

		void SetCameraTranslateSpeed(float speed) { m_CameraTranslateSpeed = speed; }
		void SetCameraRotateSpeed(float speed) { m_CameraRotateSpeed = speed; }

		SharedPtr<OrthographicCamera> GetCamera() { return m_pCamera; }
		SharedPtr<OrthographicCamera> GetCamera() const { return m_pCamera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent* event);
		bool OnWindowResized(WindowResizeEvent* event);

		bool m_Rotatable{ false };

		float m_AspectRatio{ 0.0f };
		float m_ZoomLevel{ 1.0f };

		glm::vec3 m_CameraPosition{ 0.0f, 0.0f, 0.0f };
		float m_CameraRotation{ 0.0f };
		float m_CameraTranslateSpeed{ 5.0f };
		float m_CameraRotateSpeed{ 180.0f };

		SharedPtr<OrthographicCamera> m_pCamera;
	};
}
