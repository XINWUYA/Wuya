#pragma once
#include "Wuya/Renderer/Camera.h"

namespace Wuya
{
	/* 投影相机描述 */
	struct PerspectiveCameraDesc
	{
		float Fov{ glm::radians(45.0f) };
		float Near{ 0.01f };
		float Far{ 1000.0f };

		PerspectiveCameraDesc() = default;
	};

	/* 正交相机描述 */
	struct OrthographicCameraDesc
	{
		float HeightSize{ 10.0f };
		float Near{ -1.0f };
		float Far{ 1.0f };

		OrthographicCameraDesc() = default;
	};

	/* 场景相机类 */
	class SceneCamera : public Camera
	{
	public:
		/* 投影类型 */
		enum class ProjectionType : uint8_t
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera(float aspect_ratio = 1.778f);
		~SceneCamera() override = default;

		/* 投影类型 */
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type);
		/* 相机描述 */
		const SharedPtr<PerspectiveCameraDesc>& GetPerspectiveCameraDesc() const { return m_pPerspectiveCameraDesc; }
		const SharedPtr<OrthographicCameraDesc>& GetOrthographicCameraDesc() const { return m_pOrthographicCameraDesc; }
		void SetPerspectiveCameraDesc(const SharedPtr<PerspectiveCameraDesc>& desc);
		void SetPerspectiveCameraDesc(const PerspectiveCameraDesc& desc);
		void SetOrthographicCameraDesc(const SharedPtr<OrthographicCameraDesc>& desc);
		void SetOrthographicCameraDesc(const OrthographicCameraDesc& desc);
		/* 宽高比 */
		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float ratio);

	private:
		/* 更新相机投影矩阵 */
		void UpdateProjectionMatrix();

		/* 投影类型 */
		ProjectionType m_ProjectionType{ ProjectionType::Perspective };
		/* 相机类型描述 */
		SharedPtr<PerspectiveCameraDesc> m_pPerspectiveCameraDesc{ nullptr };
		SharedPtr<OrthographicCameraDesc> m_pOrthographicCameraDesc{ nullptr };
		/* 宽高比 */
		float m_AspectRatio{ 1.778f };
	};
}
