#include "Pch.h"
#include "Camera.h"

namespace Wuya
{
	Camera::Camera(std::string name)
		: m_DebugName(std::move(name))
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
