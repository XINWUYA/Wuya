#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/* PBR����������ͼSlot */
	namespace TextureSlot
	{
		constexpr uint8_t Albedo = 0;
		constexpr uint8_t Specular = 1;
		constexpr uint8_t Normal = 2;
		constexpr uint8_t Bump = 3;
		constexpr uint8_t Displacement = 4;
		constexpr uint8_t Roughness = 5;
		constexpr uint8_t Metallic = 6;
		constexpr uint8_t Emissive = 7;
		constexpr uint8_t Ambient = 8;
		constexpr uint8_t ValidSlotCnt = 9;

		constexpr uint8_t Invalid = 255;

		inline std::string GetSlotName(uint8_t slot)
		{
			switch (slot)
			{
			case Albedo:		return "Albedo";
			case Specular:		return "Specular";
			case Normal:		return "Normal";
			case Bump:			return "Bump";
			case Displacement:	return "Displacement";
			case Roughness:		return "Roughness";
			case Metallic:		return "Metallic";
			case Emissive:		return "Emissive";
			case Ambient:		return "Ambient";
			default:			return "Invalid";
			}
		}
	}

	enum class ParamType : uint8_t
	{
		Texture = 0,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
	};

	/* ���ʲ�����Ϣ */
	struct MaterialParamInfo
	{
		ParamType	Type{}; /* �������� */
		std::string Name;   /* ������ */
		std::any	Value;  /* ����ֵ, ����Texture�����⴦����Value����Ϊstd::pair<SharedPtr<Texture>, uint32_t> */

		MaterialParamInfo() = default;
		MaterialParamInfo(ParamType type, std::string name, std::any value)
			: Type(type), Name(std::move(name)), Value(std::move(value))
		{}
	};

	/*
	 * ������
	 */
	class Material
	{
		using ParameterMap = std::unordered_map<uint32_t, MaterialParamInfo>;

	public:
		Material() = default;
		~Material();

		/* ����Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		[[nodiscard]] const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* ���ò��� */
		void SetParameters(ParamType type, const std::string& name, const std::any& param);
		[[nodiscard]] const ParameterMap& GetAllParameters() const { return m_Parameters; }
		/* �������� */
		void SetTexture(const std::string& name, const SharedPtr<Texture>& texture, int slot = -1);
		/* ���ù�դ��״̬ */
		void SetRasterState(const RenderRasterState& state) { m_RasterState = state; }
		[[nodiscard]] const RenderRasterState& GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* �󶨲����еĸ����� */
		void Bind();
		/* ������ */
		void Unbind();

		/* Ĭ�ϲ��� */
		static SharedPtr<Material>& Default();
		/* ������� */
		static SharedPtr<Material>& Error();

		/* �������� */
		static SharedPtr<Material> Create(const SharedPtr<Shader>& shader);

	private:
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* ��������ĸ��ֲ���<ToID(Name), MaterialParamInfo> */
		ParameterMap m_Parameters{};
		/* ��դ��״̬���� */
		RenderRasterState m_RasterState{};

		/* Ĭ�ϲ��ʺʹ������ */
		static SharedPtr<Material> m_pDefaultMaterial;
		static SharedPtr<Material> m_pErrorMaterial;
	};

	/* �������ࣺ
	 * ÿ��ģ�Ͷ�Ӧһ��MaterialGroup
	 * ÿ��ģ���е�MeshSegment��ӦMaterialGroup�е�һ��Material
	 */
	class MaterialGroup final
	{
	public:
		MaterialGroup() = default;
		~MaterialGroup() = default;

		/* ���һ��Material */
		void EmplaceMaterial(const SharedPtr<Material>& material);
		/* ����������ȡMaterial */
		const SharedPtr<Material>& GetMaterialByIndex(int idx);
		/* ��ȡ�������в��� */
		[[nodiscard]] const std::vector<SharedPtr<Material>>& GetAllMaterials() const { return m_Materials; }
		/* ��ղ��� */
		void ClearAllMaterials() { m_Materials.clear(); }

		/* ���л� */
		void Serializer(const std::string& path);
		/* �����л� */
		bool Deserializer(const std::string& path);

	private:
		
		/* ������·�� */
		std::string m_Path{};
		/* �����б� */
		std::vector<SharedPtr<Material>> m_Materials;
	};
}
