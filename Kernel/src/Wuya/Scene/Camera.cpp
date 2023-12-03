#include "Pch.h"
#include "Camera.h"

namespace Wuya
{
	Camera::Camera(const std::string& name, float fov, float aspect_ratio, float near_clip, float far_clip)
		: m_DebugName(name), m_Fov(fov), m_AspectRatio(aspect_ratio), m_NearClip(near_clip), m_FarClip(far_clip)
	{
		PROFILE_FUNCTION();

		m_pRenderView = CreateUniquePtr<RenderView>(m_DebugName + "_RenderView", this);
	}

	/* 更新时间戳 */
	void Camera::OnUpdate(float delta_time)
	{
		PROFILE_FUNCTION();

		ConstructRenderView();
	}
}
