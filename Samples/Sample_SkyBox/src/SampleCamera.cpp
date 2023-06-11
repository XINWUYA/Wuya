#include "Pch.h"
#include "SampleCamera.h"
#include <glm/gtx/quaternion.hpp>

namespace Wuya
{
	SampleCamera::SampleCamera(const std::string& name, float fov, float aspect_ratio, float near_clip, float far_clip)
		: Camera(name, fov, aspect_ratio, near_clip, far_clip)
	{
		PROFILE_FUNCTION();

		UpdateCameraDirections();

		UpdateProjectionMatrix();
		UpdateViewMatrix();
	}

	SampleCamera::~SampleCamera()
	{
	}

	void SampleCamera::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();

		Camera::OnUpdate(delta_time);

		if (m_IsFocus)
		{
			if (Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt))
			{
				const auto mouse_pos = Input::GetMousePos();
				const auto delta = (mouse_pos - m_LastMousePosition) * 0.003f;
				m_LastMousePosition = mouse_pos;

				if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
					OnMousePan(delta);
				else if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
					OnMouseRotate(delta);
				else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
					OnMouseZoom(delta.y);
			}
		}
		else
		{
			if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
			{
				/* 向前/向后 */
				if (Input::IsKeyPressed(Key::W))
				{
					m_Position -= m_ForwardDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}
				else if (Input::IsKeyPressed(Key::S))
				{
					m_Position += m_ForwardDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}

				/* 向左/向右 */
				if (Input::IsKeyPressed(Key::A))
				{
					m_Position -= m_RightDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}
				else if (Input::IsKeyPressed(Key::D))
				{
					m_Position += m_RightDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}

				/* 向上/向下 */
				if (Input::IsKeyPressed(Key::Q))
				{
					m_Position += m_UpDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}
				else if (Input::IsKeyPressed(Key::E))
				{
					m_Position -= m_UpDirection * m_MoveSpeed * delta_time;
					m_IsDirty = true;
				}
			}
		}
		//EDITOR_LOG_DEBUG("Camera Position: {}, {}, {}", m_Position.x, m_Position.y, m_Position.z);
		UpdateViewMatrix();
	}

	/* 设置视口区域 */
	void SampleCamera::SetViewportRegion(const ViewportRegion& region)
	{
		PROFILE_FUNCTION();

		m_ViewportRegion = region;
		m_pRenderView->SetViewportRegion(region);

		m_AspectRatio = static_cast<float>(m_ViewportRegion.Width) / static_cast<float>(m_ViewportRegion.Height);
		UpdateProjectionMatrix();

		/* 更新视口区域时，需重新构建FrameGraph, 保证RenderTarget的size是正确的 */
		m_IsFrameGraphDirty = true;
	}

	/* 构建内置的FrameGraph */
	void SampleCamera::ConstructRenderView()
	{
		PROFILE_FUNCTION();

		if (!m_IsFrameGraphDirty)
			return;

		if (m_ViewportRegion.Width <= 0 || m_ViewportRegion.Height <= 0)
			return;

		auto& frame_graph = m_pRenderView->GetFrameGraph();
		frame_graph->Reset();

		FrameGraphTexture::Descriptor color_target_desc;
		color_target_desc.Width = m_ViewportRegion.Width;
		color_target_desc.Height = m_ViewportRegion.Height;
		color_target_desc.TextureFormat = TextureFormat::RGBA8;

		FrameGraphTexture::Descriptor depth_target_desc;
		depth_target_desc.Width = m_ViewportRegion.Width;
		depth_target_desc.Height = m_ViewportRegion.Height;
		depth_target_desc.TextureFormat = TextureFormat::Depth32;

		/* Scene Pass */
		struct ScenePassData
		{
			FrameGraphResourceHandleTyped<FrameGraphTexture> SceneColor;
			FrameGraphResourceHandleTyped<FrameGraphTexture> SceneDepth;
		};

		auto scene_pass = frame_graph->AddPass<ScenePassData>("ScenePass",
			[&](FrameGraphBuilder& builder, ScenePassData& data)
			{
				builder.AsSideEffect();
			},
			[&](const FrameGraphResources& resources, const ScenePassData& data)
			{
				Renderer::GetRenderAPI()->PushDebugGroup("ScenePass");

				{
					Renderer::GetRenderAPI()->Clear();

					for (const auto& mesh_object : m_pRenderView->GetVisibleMeshObjects())
					{
						/* Fill object uniform buffer */
						Renderer::FillObjectUniformBuffer(mesh_object);

						auto& material = mesh_object.MeshSegment->GetMaterial();
						Renderer::Submit(material, mesh_object.MeshSegment->GetMeshPrimitive());
					}
				}

				Renderer::GetRenderAPI()->PopDebugGroup();
			}
			);
		// frame_graph->ExportGraphviz("framegraph.txt");
		m_pRenderView->Prepare();

		m_IsFrameGraphDirty = false;
	}

	glm::quat SampleCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	void SampleCamera::SetViewMatrix(const glm::mat4& view_mat)
	{
		PROFILE_FUNCTION();

		if (view_mat != m_ViewMatrix)
		{
			m_ViewMatrix = view_mat;

			/* 根据ViewMatrix恢复Pitch和Yaw */
			/* todo： 数据未恢复全，仍存在问题，需要重新维护这部分 */
			const auto orientation = glm::toQuat(view_mat);
			const glm::vec3 euler_angles = glm::eulerAngles(orientation);
			m_Pitch = -euler_angles.x;
			m_Yaw = -euler_angles.y;
			m_UpDirection = glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
			m_RightDirection = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));
			m_ForwardDirection = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
		}
	}

	void SampleCamera::UpdateProjectionMatrix()
	{
		PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void SampleCamera::UpdateViewMatrix()
	{
		PROFILE_FUNCTION();

		if (m_IsDirty)
		{
			if (m_IsFocus)
			{
				m_Position = m_FocalPoint - m_ForwardDirection * m_Distance; /* 相机始终围绕聚焦点 */
				m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
				m_ViewMatrix = glm::inverse(m_ViewMatrix);
			}
			else
			{
				m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
				m_ViewMatrix = glm::inverse(m_ViewMatrix);
			}

			m_IsDirty = false;
		}
	}

	void SampleCamera::UpdateCameraDirections()
	{
		PROFILE_FUNCTION();

		const auto orientation = GetOrientation();
		m_UpDirection = glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
		m_RightDirection = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));
		m_ForwardDirection = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	void SampleCamera::OnMousePan(const glm::vec2& delta)
	{
		PROFILE_FUNCTION();

		const auto speed = PanSpeed();
		m_FocalPoint += -GetRightDir() * delta.x * speed.x * m_Distance;
		m_FocalPoint += GetUpDir() * delta.y * speed.y * m_Distance;

		m_IsDirty = true;
	}

	void SampleCamera::OnMouseRotate(const glm::vec2& delta)
	{
		PROFILE_FUNCTION();

		const float yaw_sign = GetUpDir().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yaw_sign * delta.x * RotateSpeed();
		m_Pitch += delta.y * RotateSpeed();

		UpdateCameraDirections();

		m_IsDirty = true;
	}

	void SampleCamera::OnMouseZoom(float delta)
	{
		PROFILE_FUNCTION();

		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDir();
			m_Distance = 1.0f;
		}

		m_IsDirty = true;
	}

	glm::vec2 SampleCamera::PanSpeed() const
	{
		PROFILE_FUNCTION();

		const float x = std::min(m_ViewportRegion.Width / 1000.0f, 2.4f); // max = 2.4f
		const float speed_x = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float y = std::min(m_ViewportRegion.Height / 1000.0f, 2.4f); // max = 2.4f
		const float speed_y = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return glm::vec2(speed_x, speed_y);
	}

	float SampleCamera::RotateSpeed() const
	{
		return 0.8f;
	}

	float SampleCamera::ZoomSpeed() const
	{
		PROFILE_FUNCTION();

		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
}
