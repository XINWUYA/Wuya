#pragma once
#include <Wuya/Renderer/Camera.h>
#include <Wuya/Renderer/RenderCommon.h>

namespace Wuya
{
	/* �༭���ڽ������ */
	class EditorCamera final : public Camera
	{
	public:
		EditorCamera(const std::string& name = "EditorBuiltinCamera", float fov = 45.0f, float aspect_ratio = 1.778f, float near_clip = 0.1f, float far_clip = 1000.0f);
		~EditorCamera() override;

		void OnUpdate(float delta_time);
		void OnEvent(class IEvent* event) {}

		/* �����ӿ����� */
		void SetViewportRegion(const ViewportRegion& region);
		const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* ���þ۽�ģʽ */
		bool IsFocus() const { return m_IsFocus; }
		void SetFocus(bool focus) { m_IsFocus = focus; }

		/* ������� */
		void SetDistance(float distance) { m_Distance = distance; }
		float GetDistance() const { return m_Distance; }
		/* �ƶ��ٶ� */
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
		float GetMoveSpeed() const { return m_MoveSpeed; }

		void SetFocalPoint(const glm::vec3& position) { m_FocalPoint = position; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		float GetAspectRatio() const { return m_AspectRatio; }

		const glm::vec3& GetUpDir() const { return m_UpDirection; }
		const glm::vec3& GetRightDir() const { return m_RightDirection; }
		const glm::vec3& GetForwardDir() const { return m_ForwardDirection; }

		glm::quat GetOrientation() const;

		void SetViewMatrix(const glm::mat4& view_mat);

		/* ��������λ�ã���ȡEntityId */
		int32_t PickingEntityByPixelPos(uint32_t x, uint32_t y) const;

	private:
		/* �������õ�FrameGraph */
		void ConstructRenderView() override;

		void UpdateProjectionMatrix();
		void UpdateViewMatrix();
		void UpdateCameraDirections();

		// �۽�ģʽ
		void OnMousePan(const glm::vec2& delta); // ����ƽ��
		void OnMouseRotate(const glm::vec2& delta); // �ƾ۽�������ת
		void OnMouseZoom(float delta); // ��Զ����

		glm::vec2 PanSpeed() const;
		float RotateSpeed() const;
		float ZoomSpeed() const;

		/* ����������� */
		float m_Fov{ 45.0f };
		float m_AspectRatio{ 1.778f };
		float m_NearClip{ 0.1f };
		float m_FarClip{ 1000.0f };

		float m_Pitch{ 0.0f }, m_Yaw{ 0.0f };
		/* ���λ�� */
		float m_Distance{ 10.0f };
		/* ����ƶ��ٶ� */
		float m_MoveSpeed{ 1.0f };

		/* �Ƿ����þ۽�ģʽ */
		bool m_IsFocus{ true };
		/* �Ƿ���Ҫ���±任���� */
		bool m_IsDirty{ true };
		/* �Ƿ���Ҫ���¹���FrameGraph */
		bool m_IsFrameGraphDirty = true;

		/* ������� */
		glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_RightDirection{ 1.0f, 0.0f, 0.0f };
		glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };

		glm::vec3 m_FocalPoint{ 0.0f, 0.0f, 0.0f };
		glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

		/* �ӿ����� */
		ViewportRegion m_ViewportRegion{};
	};
}
