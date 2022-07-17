#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	/* ��׼�����ͶӰ�����*/
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		/* ���λ�� */
		const glm::vec3& GetPosition() const { return m_Position; }

		/* ������Ϣ */
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

	protected:
		/* ���λ�� */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* ������� */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };
	};
}
