#pragma once
#include <glm/glm.hpp>
#include "Wuya/Renderer/RenderView.h"

namespace Wuya
{
	/* ��׼�����ͶӰ�����*/
	class Camera
	{
	public:
		Camera(const std::string& name, float fov = 45.0f, float aspect_ratio = 1.778f, float near_clip = 0.1f, float far_clip = 1000.0f);
		virtual ~Camera() = default;

		/* ����ʱ��� */
		virtual void OnUpdate(float delta_time);

		/* ���λ�� */
		[[nodiscard]]
		const glm::vec3& GetPosition() const { return m_Position; }
		virtual void SetPosition(const glm::vec3& position) { m_Position = position; }

		/* ������� */
		[[nodiscard]]
		const glm::vec3& GetUpDir() const { return m_UpDirection; }
		[[nodiscard]]
		const glm::vec3& GetRightDir() const { return m_RightDirection; }
		[[nodiscard]]
		const glm::vec3& GetForwardDir() const { return m_ForwardDirection; }

		[[nodiscard]]
		float GetAspectRatio() const { return m_AspectRatio; }
		[[nodiscard]]
		float GetFov() const { return m_Fov; }

		/* ������Ϣ */
		[[nodiscard]]
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		[[nodiscard]]
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]]
		glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		/* ��ȡRenderView */
		[[nodiscard]]
		RenderView* GetRenderView() const { return m_pRenderView.get(); }

	protected:
		/* Ϊ��ǰ�������RenderView:
		 * ��������֯��ǰRenderView��FrameGraph,
		 * ÿ�����override�ú�������Ϊ��ͬ�����ʹ�ò�ͬ��FrameGraph��
		 */
		virtual void ConstructRenderView() {}

		/* ����� */
		std::string m_DebugName{ "Unnamed Camera" };

		/* ����������� */
		float m_Fov{ 45.0f };
		float m_AspectRatio{ 1.778f };
		float m_NearClip{ 0.1f };
		float m_FarClip{ 1000.0f };

		/* ���λ�� */
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };

		/* ������� */
		glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_RightDirection{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };

		/* ������� */
		glm::mat4 m_ViewMatrix{ glm::mat4(1.0f) };
		glm::mat4 m_ProjectionMatrix{ glm::mat4(1.0f) };

		/* ÿ�������Ӧһ��RenderView */
		UniquePtr<RenderView> m_pRenderView{ nullptr };
	};
}
