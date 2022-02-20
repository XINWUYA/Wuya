#include "pch.h"
#include "OrthographicCameraController.h"
#include "OrthographicCamera.h"
#include "Wuya/Core/Input.h"
#include "Wuya/Events/MouseEvent.h"
#include "Wuya/Events/ApplicationEvent.h"

namespace Wuya
{
	OrthographicCameraController::OrthographicCameraController(float aspect_ratio, bool rotatable)
		: m_AspectRatio(aspect_ratio), m_Rotatable(rotatable)
	{
		m_pCamera = CreateSharedPtr<OrthographicCamera>(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void OrthographicCameraController::OnUpdate(float delta_time)
	{
		if (Input::IsKeyPressed(Key::W)) // ����
		{
			m_CameraPosition.x += sin(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
		}
		else if (Input::IsKeyPressed(Key::S)) // ����
		{
			m_CameraPosition.x -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
		}
		else if (Input::IsKeyPressed(Key::A)) // ����
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
		}
		else if (Input::IsKeyPressed(Key::D)) // ����
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslateSpeed * delta_time;
		}

		if (m_Rotatable)
		{
			if (Input::IsKeyPressed(Key::Q)) // ��ʱ��
			{
				m_CameraRotation += m_CameraRotateSpeed * delta_time;
			}
			else if (Input::IsKeyPressed(Key::E)) // ˳ʱ��
			{
				m_CameraRotation -= m_CameraRotateSpeed * delta_time;
			}

			// ��m_CameraRotation������-180.0f~180.0f����ֹ�������
			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_pCamera->SetRotation(m_CameraRotation);
		}

		m_pCamera->SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnEvent(IEvent* event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_pCamera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent* event)
	{
		m_ZoomLevel -= event->GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_pCamera->SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return true;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent* event)
	{
		OnResize(static_cast<float>(event->GetWidth()), static_cast<float>(event->GetHeight()));
		return true;
	}
}
