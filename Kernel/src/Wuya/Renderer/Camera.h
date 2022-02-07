#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	// 标准相机（投影相机）
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	protected:
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };
	};

	// 正交相机
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float top, float bottom);
		~OrthographicCamera() = default;
	};
}
