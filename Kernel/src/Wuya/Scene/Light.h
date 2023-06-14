#pragma once
#include "SceneCommon.h"

namespace Wuya
{
	/* 光源类型 */
	enum class LightType : uint8_t
	{
		Directional,
		Point,
		Spot,
		Area,
		Volume
	};

	/* 光源类 */
	class Light
	{
	public:
		COMPONENT_CLASS(Light)

		Light() = default;
		virtual ~Light() = default;

		/* 获取光源类型 */
		[[nodiscard]] virtual LightType GetLightType() const = 0;

		/* 光源颜色 */
		void SetColor(const glm::vec4& color) { m_Color = color; }
		[[nodiscard]] const glm::vec4& GetColor() const { return m_Color; }

		/* 光源强度 */
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		[[nodiscard]] float GetIntensity() const { return m_Intensity; }

		/* 是否投影 */
		void SetIsCastShadow(bool enable) { m_IsCastShadow = enable; }
		[[nodiscard]] bool IsCastShadow() const { return m_IsCastShadow; }

		/* 创建指定类型光源 */
		static SharedPtr<Light> Create(LightType type);

	protected:
		/* 标记名 */
		std::string m_DebugName{ "Unnamed Light" };
		/* 光源颜色 */
		glm::vec4 m_Color{ 1.0f };
		/* 光源强度 */
		float m_Intensity{ 1.0f };
		/* 是否投影 */
		bool m_IsCastShadow{ false };
		/* 光源位置 */
		glm::vec3 m_LightPos{ 0.0f };
		/* 光源方向 */
		glm::vec3 m_LightDir{0.0f, -1.0f, 0.0f};
	};


	/* 方向光 */
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight() = default;
		~DirectionalLight() override = default;

		/* 获取光源类型 */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Directional; }

	private:

	};


	/* 点光 */
	class PointLight final : public Light
	{
	public:
		PointLight() = default;
		~PointLight() override = default;

		/* 获取光源类型 */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Point; }
	};


	/* 聚光 */
	class SpotLight final : public Light
	{
	public:
		SpotLight() = default;
		~SpotLight() override = default;

		/* 获取光源类型 */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Spot; }
	};


	/* 面光 */
	class AreaLight final : public Light
	{
	public:
		AreaLight() = default;
		~AreaLight() override = default;

		/* 获取光源类型 */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Area; }
	};


	/* 体积光 */
	class VolumeLight final : public Light
	{
	public:
		VolumeLight() = default;
		~VolumeLight() override = default;

		/* 获取光源类型 */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Volume; }
	};

}