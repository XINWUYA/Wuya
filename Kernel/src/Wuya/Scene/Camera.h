#pragma once
#include <glm/glm.hpp>
#include "Wuya/Renderer/RenderView.h"

namespace Wuya
{
	/* ��׼�����ͶӰ�����*/
	class Camera
	{
	public:
		Camera(std::string name);
		virtual ~Camera() = default;

		/* ����ʱ��� */
		virtual void OnUpdate(float delta_time);

		/* ���λ�� */
		[[nodiscard]] const glm::vec3& GetPosition() const { return m_Position; }

		/* ������Ϣ */
		[[nodiscard]] const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]] glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		/* ��ȡRenderView */
		[[nodiscard]] RenderView* GetRenderView() const { return m_pRenderView.get(); }

	protected:
		/* Ϊ��ǰ�������RenderView:
		 * ��������֯��ǰRenderView��FrameGraph,
		 * ÿ�����override�ú�������Ϊ��ͬ�����ʹ�ò�ͬ��FrameGraph��
		 */
		virtual void ConstructRenderView() {}

		/* ����� */
		std::string m_DebugName{ "Unnamed Camera" };
		/* ���λ�� */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* ������� */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* ÿ�������Ӧһ��RenderView */
		UniquePtr<RenderView> m_pRenderView{ nullptr };
	};
}
