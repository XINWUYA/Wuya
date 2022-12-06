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
		constexpr uint8_t Roughness = 3;
		constexpr uint8_t Metallic = 4;
		constexpr uint8_t Emissive = 5;
		constexpr uint8_t Ambient = 6;
		constexpr uint8_t ValidSlotCnt = 7;

		inline std::string GetSlotName(uint8_t slot)
		{
			switch (slot)
			{
			case Albedo:	return "Albedo";
			case Specular:	return "Specular";
			case Normal:	return "Normal";
			case Roughness: return "Roughness";
			case Metallic:	return "Metallic";
			case Emissive:	return "Emissive";
			case Ambient:	return "Ambient";
			default:		return "Unknown";
			}
		}
	}

	/*
	 * ������
	 */
	class Material
	{
		using ParameterMap = std::unordered_map<std::string, std::any>;
		using TextureMap = std::unordered_map<SharedPtr<Texture>, uint32_t>;

	public:
		Material() = default;
		~Material();

		/* ����Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		[[nodiscard]] const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* ���ò��� */
		void SetParameters(const std::string& name, const std::any& param);
		[[nodiscard]] const ParameterMap& GetAllParameters() const { return m_Parameters; }
		/* �������� */
		void SetTexture(const SharedPtr<Texture>& texture, uint32_t slot);
		[[nodiscard]] const SharedPtr<Texture>& GetTextureBySlot(uint32_t slot) const;
		/* ���ù�դ��״̬ */
		void SetRasterState(RenderRasterState state) { m_RasterState = state; }
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
		/* ��������ĸ��ֲ���<Name, Vale> */
		ParameterMap m_Parameters{};
		/* ��������ĸ�������<Texture��Slot> */
		TextureMap m_Textures{};
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
		MaterialGroup(std::string path);
		~MaterialGroup() = default;

		/* ���һ��Material */
		void EmplaceMaterial(const SharedPtr<Material>& material);
		/* ����������ȡMaterial */
		const SharedPtr<Material>& GetMaterialByIndex(int idx);
		/* ��ȡ�������в��� */
		const std::vector<SharedPtr<Material>>& GetAllMaterials() const { return m_Materials; }

	private:
		/* �����л� */
		bool Deserializer();

		/* ������·�� */
		std::string m_Path{};
		/* �����б� */
		std::vector<SharedPtr<Material>> m_Materials;
	};
}
