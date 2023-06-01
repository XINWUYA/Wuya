#pragma once
#include "SceneCommon.h"

namespace Wuya
{
	/* ��Դ���� */
	enum class LightType : uint8_t
	{
		Directional,
		Point,
		Spot,
		Area,
		Volume
	};

	/* ��Դ�� */
	class Light
	{
	public:
		COMPONENT_CLASS(Light)

		Light() = default;
		virtual ~Light() = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] virtual LightType GetLightType() const = 0;

		/* ��Դ��ɫ */
		void SetColor(const glm::vec4& color) { m_Color = color; }
		[[nodiscard]] const glm::vec4& GetColor() const { return m_Color; }

		/* ��Դǿ�� */
		void SetIntensity(float intensity) { m_Intensity = intensity; }
		[[nodiscard]] float GetIntensity() const { return m_Intensity; }

		/* �Ƿ�ͶӰ */
		void SetIsCastShadow(bool enable) { m_IsCastShadow = enable; }
		[[nodiscard]] bool IsCastShadow() const { return m_IsCastShadow; }

		/* ����ָ�����͹�Դ */
		static SharedPtr<Light> Create(LightType type);

	protected:
		/* ����� */
		std::string m_DebugName{ "Unnamed Light" };
		/* ��Դ��ɫ */
		glm::vec4 m_Color{ 1.0f };
		/* ��Դǿ�� */
		float m_Intensity{ 1.0f };
		/* �Ƿ�ͶӰ */
		bool m_IsCastShadow{ false };
	};


	/* ����� */
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight() = default;
		~DirectionalLight() override = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Directional; }

	private:

	};


	/* ��� */
	class PointLight final : public Light
	{
	public:
		PointLight() = default;
		~PointLight() override = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Point; }
	};


	/* �۹� */
	class SpotLight final : public Light
	{
	public:
		SpotLight() = default;
		~SpotLight() override = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Spot; }
	};


	/* ��� */
	class AreaLight final : public Light
	{
	public:
		AreaLight() = default;
		~AreaLight() override = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Area; }
	};


	/* ����� */
	class VolumeLight final : public Light
	{
	public:
		VolumeLight() = default;
		~VolumeLight() override = default;

		/* ��ȡ��Դ���� */
		[[nodiscard]] LightType GetLightType() const override { return LightType::Volume; }
	};

}