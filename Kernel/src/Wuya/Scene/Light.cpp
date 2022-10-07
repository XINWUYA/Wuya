#include "Pch.h"
#include "Light.h"

namespace Wuya
{
	/* ����ָ�����͹�Դ */
	SharedPtr<Light> Light::Create(LightType type)
	{
		switch (type)
		{
		case LightType::Directional:	return CreateSharedPtr<DirectionalLight>();
		case LightType::Point:			return CreateSharedPtr<PointLight>();
		case LightType::Spot:			return CreateSharedPtr<SpotLight>();
		case LightType::Area:			return CreateSharedPtr<AreaLight>();
		case LightType::Volume:			return CreateSharedPtr<VolumeLight>();
		}
		return nullptr;
	}
}
