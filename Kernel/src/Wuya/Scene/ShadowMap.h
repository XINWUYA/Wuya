#pragma once
#include "Light.h"
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Scene;

	/* 每个光源对应一个ShadowMap */
	class ShadowMap
	{
	public:
		ShadowMap(const SharedPtr<Light>& light, uint16_t shadow_idx, uint8_t face_idx);
		~ShadowMap() = default;

		/* 禁止拷贝 */
		ShadowMap(const ShadowMap&) = delete;
		ShadowMap& operator=(const ShadowMap&) = delete;

		[[nodiscard]] uint16_t GetShadowIndex() const { return m_ShadowIndex; }

		void SetLayer(uint8_t layer) { m_Layer = layer; }
		[[nodiscard]] uint8_t GetLayer() const { return m_Layer; }

		static glm::mat4 GetDirectionalLightViewMatrix(const glm::vec3& direction, const glm::vec3& origin = {}) noexcept;
		static glm::mat4 GetPunctualLightViewMatrix(uint8_t face_idx, const glm::vec3& origin) noexcept;

	private:
		SharedPtr<Light> m_pLight{ nullptr };
		uint16_t m_ShadowIndex{ 0 };
		uint8_t m_FaceIndex{ 0 };

		/* Shadow在TextureArray中的对应MipLayer */
		uint8_t m_Layer{ 0 };

		friend class ShadowMapManager;
	};

	/* 所有光源的RT将被收集到同一个TextureArray中 */
	class ShadowMapManager
	{
	public:
		ShadowMapManager() = default;
		~ShadowMapManager() = default;

		void AddShadowMap(const SharedPtr<Light>& light);
		void PrepareAllShadowMaps(const SharedPtr<Scene>& scene);

	private:
		void PrepareRequiredTexture();

		struct ShadowMapTextureDesc
		{
			uint32_t Size{ 0 };
			uint8_t LayerNum{ 0 };
			uint8_t MipLevels{ 0 };
			TextureFormat Format{ TextureFormat::Depth16 };
		};

		const uint8_t SHADOW_CASCADE_MAX_NUM = 4;
		const uint8_t SHADOW_PUNCTUAL_MAX_NUM = 60; // 64 - 4

		std::vector<SharedPtr<ShadowMap>> m_CascadeShadowMaps{};
		std::vector<SharedPtr<ShadowMap>> m_PunctualShadowMaps{};

		/* ShadowMapTexture所需的信息 */
		ShadowMapTextureDesc m_RequiredTextureDesc{};
		uint32_t m_MaxDimension{ 128 };
	};
}
