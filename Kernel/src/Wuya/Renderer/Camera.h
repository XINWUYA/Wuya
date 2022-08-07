#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class RenderView;

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

		/* 为当前相机构建RenderView:
		 * 在这里组织当前RenderView的FrameGraph,
		 * 每个相机override该函数，以为不同的相机使用不同的FrameGraph。
		 */
		virtual void ConstructRenderView() {}

		/* 绑定一个已有的RenderView */
		void BindRenderView(const SharedPtr<RenderView>& render_view) { m_pRenderView = render_view; }
		/* 获取RenderView */
		const SharedPtr<RenderView>& GetRenderView() const { return m_pRenderView; }

	protected:
		/* 相机位置 */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* 相机矩阵 */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* 每个相机对应一个RenderView */
		SharedPtr<RenderView> m_pRenderView{ nullptr };
	};
}
