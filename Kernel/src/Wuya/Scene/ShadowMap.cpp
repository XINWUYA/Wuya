#include "Pch.h"
#include "ShadowMap.h"
#include "Scene.h"

namespace Wuya
{
	ShadowMap::ShadowMap(const SharedPtr<Light>& light, uint16_t shadow_idx, uint8_t face_idx)
		: m_pLight(light), m_ShadowIndex(shadow_idx), m_FaceIndex(face_idx)
	{
	}

	glm::mat4 ShadowMap::GetDirectionalLightViewMatrix(const glm::vec3& direction, const glm::vec3& origin) noexcept
	{
		auto up = glm::vec3(0.0f, 1.0f, 0.0f);
		auto front = direction;
		if (std::abs(glm::dot(front, up)) > 0.999f)
		{ // looking straight up
			up = { up.z, up.x, up.y };
		}

		auto right = glm::normalize(glm::cross(front, up));
		up = glm::cross(right, front);

		const glm::mat4 transform = {
			glm::vec4(right, 0.0f),
			glm::vec4(up, 0.0f),
			glm::vec4(-front, 0.0f),
			glm::vec4(origin, 1.0f)
		};

		return glm::inverse(transform);
	}

	glm::mat4 ShadowMap::GetPunctualLightViewMatrix(uint8_t face_idx, const glm::vec3& origin) noexcept
	{
		glm::vec3 direction;
		if (face_idx == 0) direction = { 1.0f, 0.0f, 0.0f }; // PositiveX
		else if (face_idx == 1) direction = { -1.0f, 0.0f, 0.0f }; // NegativeX
		else if (face_idx == 2) direction = { 0.0f, 1.0f, 0.0f }; // PositionY
		else if (face_idx == 3) direction = { 0.0f, -1.0f, 0.0f }; // NegativeY
		else if (face_idx == 4) direction = { 0.0f, 0.0f, 1.0f }; // PositiveZ
		else if (face_idx == 5) direction = { 0.0f, 0.0f, -1.0f }; // -PositionZ

		return GetDirectionalLightViewMatrix(direction, origin);
	}

	void ShadowMapManager::AddShadowMap(const SharedPtr<Light>& light)
	{
		if (!light || !light->IsCastShadow())
			return;

		if (auto& shadow_map_info = light->GetShadowMapInfo())
		{
			switch (light->GetLightType())
			{
			case LightType::Directional:
				ASSERT(shadow_map_info->CascadeCnt <= SHADOW_CASCADE_MAX_NUM);
				for (uint8_t cascade_id = 0; cascade_id < shadow_map_info->CascadeCnt; ++cascade_id)
				{
					auto shadow_map = CreateSharedPtr<ShadowMap>(light, cascade_id, 0);
					m_CascadeShadowMaps[cascade_id] = shadow_map;
				}
				break;
			case LightType::Point:
				
				/* todo */
				break;
			case LightType::Spot:
				/* todo: */
				break;
			default:
				break;
			}

			/* 阴影纹理大小 */
			m_MaxDimension = std::max(m_MaxDimension, shadow_map_info->Size);
		}
	}

	/* 准备ShadowMapArray, 先存放方向光的Shadow */
	void ShadowMapManager::PrepareAllShadowMaps(const SharedPtr<Scene>& scene)
	{
		PrepareRequiredTexture();

		/* 准备方向光的ShadowMap */
		if (!m_CascadeShadowMaps.empty())
		{
			auto& shadow_map = m_CascadeShadowMaps[0];
			auto light = std::dynamic_pointer_cast<DirectionalLight>(shadow_map->m_pLight);
			auto& direction = light->GetDirection();
			auto light_view_mat = shadow_map->GetDirectionalLightViewMatrix(direction);

			// todo: culling
		}
	}

	void ShadowMapManager::PrepareRequiredTexture()
	{
		uint8_t layer = 0;
		uint32_t max_dimension = 0;

		/* 方向光 */
		for (auto& shadow_map : m_CascadeShadowMaps)
		{
			shadow_map->SetLayer(layer++);
		}

		/* 点光/聚光 */
		for (auto& shadow_map : m_PunctualShadowMaps)
		{
			shadow_map->SetLayer(layer++);
		}

		const uint8_t total_layer_num = layer;
		m_RequiredTextureDesc = { max_dimension, total_layer_num, 1, TextureFormat::Depth16 };
	}
}
