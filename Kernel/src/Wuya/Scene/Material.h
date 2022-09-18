#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/*
	 * ������
	 */
	class Material
	{
	public:
		Material() = default;
		~Material();

		/* ����Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* ���ò��� */
		void SetParameters(const std::string& name, const std::any& param);
		/* �������� */
		void SetTexture(const SharedPtr<Texture>& texture, uint32_t slot);
		/* ���ù�դ��״̬ */
		void SetRasterState(RenderRasterState state) { m_RasterState = state; }
		RenderRasterState GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* �󶨲����еĸ����� */
		void Bind();
		/* ������ */
		void Unbind();

		/* Ĭ�ϲ��� */
		static SharedPtr<Material> Default();
		/* ������� */
		static SharedPtr<Material> Error();

	private:
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* ��������ĸ��ֲ���<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* ��������ĸ�������<Texture��Slot> */
		std::unordered_map<SharedPtr<Texture>, uint32_t> m_Textures{};
		/* ��դ��״̬���� */
		RenderRasterState m_RasterState{};
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
