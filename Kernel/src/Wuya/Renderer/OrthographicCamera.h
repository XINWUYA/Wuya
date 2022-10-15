#pragma once
#include "Wuya/Renderer/Camera.h"

namespace Wuya
{
	// 正交相机
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(const std::string& name, float left, float right, float top, float bottom);
		~OrthographicCamera() override;

		void SetProjection(float left, float right, float top, float bottom);
		void SetPosition(const glm::vec3& position);
		void SetRotation(float rotation);

		float GetRotation() const { return m_Rotation; }

	private:
		void UpdateCameraMatrix();

		float m_Rotation{ 0.0f };
	};
}
