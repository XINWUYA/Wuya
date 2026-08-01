#pragma once
#include "Light.h"
#include "Helios/Renderer/RenderCommon.h"

namespace Helios
{
	class Scene;
	class RenderView;
	class FrameGraph;
	class DeviceTexture;
	class DeviceShader;
	class Camera;

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

		/* 级联阴影的光照视图投影矩阵 */
		void SetLightViewProjectionMat(const glm::mat4& vp_mat) { m_LightViewProjectionMat = vp_mat; }
		[[nodiscard]] const glm::mat4& GetLightViewProjectionMat() const { return m_LightViewProjectionMat; }

		static glm::mat4 GetDirectionalLightViewMatrix(const glm::vec3& direction, const glm::vec3& origin = {}) noexcept;
		static glm::mat4 GetPunctualLightViewMatrix(uint8_t face_idx, const glm::vec3& origin) noexcept;

	private:
		SharedPtr<Light> m_pLight{ nullptr };
		uint16_t m_ShadowIndex{ 0 };
		uint8_t m_FaceIndex{ 0 };

		/* Shadow在TextureArray中的对应MipLayer */
		uint8_t m_Layer{ 0 };

		/* 级联阴影的光照视图投影矩阵 */
		glm::mat4 m_LightViewProjectionMat{ 1.0f };

		friend class ShadowMapManager;
	};

	/* 所有光源的RT将被收集到同一个TextureArray中 */
	class ShadowMapManager
	{
	public:
		ShadowMapManager() = default;
		~ShadowMapManager() = default;

		void RegisterShadowLight(const SharedPtr<Light>& light);
		void PrepareForShadowMaps(const SharedPtr<Scene>& scene, const Camera* camera);

		/* 依据当前方向光方向与相机，重算各级联的视图投影矩阵与分割距离。
		 * 与 PrepareForShadowMaps 中的矩阵计算逻辑一致，但作为独立接口暴露，
		 * 供每帧执行前调用，使方向光旋转 / 相机移动能实时反映到阴影投影矩阵。 */
		void UpdateCascadeMatrices(const Camera* camera);

		/* 清空已收集的阴影贴图与纹理，保留Manager对象本身 */
		void Clear();

		/* 将ShadowPass注入到指定的FrameGraph中
		 * 阴影纹理句柄会被写入FrameGraph的Blackboard（"ShadowMapHandle"），供后续Pass使用
		 * render_view 用于执行阶段获取当前可见的网格对象（RenderView为UniquePtr，故使用原始指针）
		 */
		void AddShadowPass(FrameGraph& frame_graph, const SharedPtr<Scene>& scene, RenderView* render_view);

		/* 获取阴影纹理 */
		[[nodiscard]] SharedPtr<DeviceTexture> GetShadowMapTexture() const { return m_ShadowMapTexture; }
		/* 获取级联阴影贴图 */
		[[nodiscard]] const std::vector<SharedPtr<ShadowMap>>& GetCascadeShadowMaps() const { return m_CascadeShadowMaps; }

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

		/* 阴影纹理数组 */
		SharedPtr<DeviceTexture> m_ShadowMapTexture{ nullptr };
		SharedPtr<DeviceShader> m_ShadowCasterShader{ nullptr };

		/* 各级联在视图空间的分割距离（远边界），由 PrepareAllShadowMaps 计算，供 AddShadowPass 填充 UBO */
		glm::vec4 m_CascadeSplits{ 0.0f };

		/* 脏标记：首次准备 / 纹理重设后必须重算级联矩阵。
		 * 平时由 UpdateCascadeMatrices 内部对比缓存（方向光方向 + 相机视图/投影矩阵）决定是否需要重算，
		 * 避免方向光与相机均静止时每帧重复计算 tight-fit 正交矩阵。 */
		bool m_IsDirty{ true };
		glm::vec3 m_LastLightDir{ 0.0f };
		glm::mat4 m_LastViewMat{ 1.0f };
		glm::mat4 m_LastProjMat{ 1.0f };
	};
}
