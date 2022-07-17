#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	/* 标准相机（投影相机）*/
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		/* 相机位置 */
		const glm::vec3& GetPosition() const { return m_Position; }

		/* 矩阵信息 */
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

	protected:
		/* 相机位置 */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* 相机矩阵 */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };
	};
}
