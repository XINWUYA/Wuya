#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class RenderView;

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

		/* Ϊ��ǰ�������RenderView:
		 * ��������֯��ǰRenderView��FrameGraph,
		 * ÿ�����override�ú�������Ϊ��ͬ�����ʹ�ò�ͬ��FrameGraph��
		 */
		virtual void ConstructRenderView() {}

		/* ��һ�����е�RenderView */
		void BindRenderView(const SharedPtr<RenderView>& render_view) { m_pRenderView = render_view; }
		/* ��ȡRenderView */
		const SharedPtr<RenderView>& GetRenderView() const { return m_pRenderView; }

	protected:
		/* ���λ�� */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* ������� */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* ÿ�������Ӧһ��RenderView */
		SharedPtr<RenderView> m_pRenderView{ nullptr };
	};
}
