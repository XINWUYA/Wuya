#include "Pch.h"
#include "Components.h"
#include "Scene.h"

namespace Helios
{
	
	/* ReflectionProbeComponent构造时注册到ReflectionProbeManager */
	void ReflectionProbeComponent::OnAdded(const Scene& scene, Entity& entity)
	{
		if (auto* manager = scene.GetReflectionProbeManager().get())
			if (m_ReflectionProbe)
				manager->RegisterProbe(m_ReflectionProbe);
	}

	/* ReflectionProbeComponent移出时从ReflectionProbeManager中移除 */
	void ReflectionProbeComponent::OnRemoved(const Scene& scene, Entity& entity)
	{
		if (auto* manager = scene.GetReflectionProbeManager().get())
			if (m_ReflectionProbe)
				manager->UnregisterProbe(m_ReflectionProbe);
	}
}
