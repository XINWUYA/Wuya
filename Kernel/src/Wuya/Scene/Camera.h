#pragma once
#include <glm/glm.hpp>
#include "Wuya/Renderer/RenderView.h"

namespace Wuya
{
	/* 标准相机（投影相机）*/
	class Camera
	{
	public:
		Camera(const std::string& name, float fov = 45.0f, float aspect_ratio = 1.778f, float near_clip = 0.1f, float far_clip = 1000.0f);
		virtual ~Camera() = default;

		/* 更新时间戳 */
		virtual void OnUpdate(float delta_time);

		/* 相机位置 */
		[[nodiscard]]
		virtual const glm::vec3& GetPosition() const { return m_Position; }
		virtual void SetPosition(const glm::vec3& position) { m_Position = position; }

		/* 相机方向 */
		[[nodiscard]]
		virtual const glm::vec3& GetUpDir() const { return m_UpDirection; }
		[[nodiscard]]
		virtual const glm::vec3& GetRightDir() const { return m_RightDirection; }
		[[nodiscard]]
		virtual const glm::vec3& GetForwardDir() const { return m_ForwardDirection; }

		[[nodiscard]]
		virtual float GetAspectRatio() const { return m_AspectRatio; }
		[[nodiscard]]
		virtual float GetFov() const { return m_Fov; }

		[[nodiscard]]
		virtual float GetNearClip() const { return m_NearClip; }
		virtual void SetNearClip(float near_clip) { m_NearClip = near_clip; }

		[[nodiscard]]
		virtual float GetFarClip() const { return m_FarClip; }
		virtual void SetFarClip(float far_clip) { m_FarClip = far_clip; }

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

		/* 标记名 */
		std::string m_DebugName{ "Unnamed Camera" };

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

		/* 相机矩阵 */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* 每个相机对应一个RenderView */
		UniquePtr<RenderView> m_pRenderView{ nullptr };
	};
}
