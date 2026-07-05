#pragma once
#include <glm/glm.hpp>
#include "Helios/Renderer/RenderView.h"
#include "SceneCommon.h"

namespace Helios
{
	/* 投影类型 */
	enum class CameraProjectionType : uint8_t
	{
		Perspective = 0, // 透视
		Orthographic = 1 // 正交
	};

	/* 相机类 */
	class Camera
	{
	public:
		COMPONENT_CLASS(Camera)
			Camera(CameraProjectionType type = CameraProjectionType::Perspective, const std::string& name = "New Camera", float fov = 45.0f, float aspect_ratio = 1.778f, float near = 0.01f, float far = 1000.0f);
		virtual ~Camera() = default;

		/* 更新时间戳 */
		virtual void OnUpdate(float delta_time);

		/* 投影类型 */
		CameraProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(CameraProjectionType type);

	/* 相机位置 */
	const glm::vec3& GetPosition() const { return m_Position; }
	void SetPosition(const glm::vec3& position);

	/* 根据世界变换矩阵更新相机位置、朝向和视图矩阵 */
	void SetTransform(const glm::mat4& transform);

	/* 相机旋转（正交相机绕 Z 轴，角度制） */
	float GetRotation() const { return m_Rotation; }
	void SetRotation(float rotation);

		/* 相机方向 */
		const glm::vec3& GetUpDir() const { return m_UpDirection; }
		const glm::vec3& GetRightDir() const { return m_RightDirection; }
		const glm::vec3& GetForwardDir() const { return m_ForwardDirection; }

		/* 宽高比 */
		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspect_ratio) { m_AspectRatio = aspect_ratio; m_IsDirty = true; }

		/* Field of View */
		float GetFov() const { return m_Fov; }
		void SetFov(float fov) { m_Fov = fov; m_IsDirty = true; }

		/* 相机近平面 */
		float GetNearClip() const { return m_NearClip; }
		void SetNearClip(float near_clip) { m_NearClip = near_clip; m_IsDirty = true; }

		/* 相机原平面 */
		float GetFarClip() const { return m_FarClip; }
		void SetFarClip(float far_clip) { m_FarClip = far_clip; m_IsDirty = true; }

	/* 高度区域（正交相机） */
	float GetHeightSize() const { return m_HeightSize; }
	void SetHeightSize(float height_size) { m_HeightSize = height_size; m_IsDirty = true; }

	/* 聚焦模式 */
	bool IsFocus() const { return m_IsFocus; }
	void SetFocus(bool focus);

	const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }
	void SetFocalPoint(const glm::vec3& focal_point) { m_FocalPoint = focal_point; m_IsDirty = true; }

	float GetFocalDistance() const { return m_FocalDistance; }
	void SetFocalDistance(float distance) { m_FocalDistance = distance; m_IsDirty = true; }

	/* 矩阵信息 */
		[[nodiscard]]
		virtual const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]]
		virtual const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]]
		virtual glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		/* 获取RenderView */
		[[nodiscard]]
		virtual RenderView* GetRenderView() const { return m_pRenderView.get(); }

	protected:
		/* 为当前相机构建RenderView:
		 * 在这里组织当前RenderView的FrameGraph,
		 * 每个相机override该函数，以为不同的相机使用不同的FrameGraph。
		 */
		virtual void ConstructRenderView() {}
		/* 更新相机视图矩阵 */
		virtual void UpdateViewMatrix();
		/* 更新相机投影矩阵 */
		virtual void UpdateProjectionMatrix();

		/* 标记名 */
		std::string m_DebugName{ "Unnamed Camera" };

		/* 相机类型 */
		CameraProjectionType m_ProjectionType{ CameraProjectionType::Perspective };

		/* 相机基本参数 */
		float m_Fov{ 45.0f };
		float m_AspectRatio{ 1.778f };
		float m_NearClip{ 0.1f };
		float m_FarClip{ 1000.0f };

		/* 相机位置 */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* 相机朝向 */
		glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_RightDirection{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };

		/* 正交相机区域 */
		float m_HeightSize{ 10.0f };

		/* 相机旋转（正交相机绕 Z 轴，角度制） */
		float m_Rotation{ 0.0f };

		/* 聚焦模式 */
		bool m_IsFocus{ false };
		float m_FocalDistance{ 10.0f };
		glm::vec3 m_FocalPoint{ 0.0f, 0.0f, 0.0f };

		/* 相机矩阵 */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* 每个相机对应一个RenderView */
		UniquePtr<RenderView> m_pRenderView{ nullptr };
		bool m_IsDirty{ true };
	};
}
