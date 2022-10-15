#pragma once
#include <glm/glm.hpp>
#include "Wuya/Renderer/RenderView.h"

namespace Wuya
{
	/* 标准相机（投影相机）*/
	class Camera
	{
	public:
		Camera(std::string name);
		virtual ~Camera() = default;

		/* 更新时间戳 */
		virtual void OnUpdate(float delta_time);

		/* 相机位置 */
		[[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }

		/* 矩阵信息 */
		[[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]] glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		/* 获取RenderView */
		[[nodiscard]] RenderView* GetRenderView() const { return m_pRenderView.get(); }

	protected:
		/* 为当前相机构建RenderView:
		 * 在这里组织当前RenderView的FrameGraph,
		 * 每个相机override该函数，以为不同的相机使用不同的FrameGraph。
		 */
		virtual void ConstructRenderView() {}

		/* 标记名 */
		std::string m_DebugName{ "Unnamed Camera" };
		/* 相机位置 */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* 相机矩阵 */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* 每个相机对应一个RenderView */
		UniquePtr<RenderView> m_pRenderView{ nullptr };
	};
}
