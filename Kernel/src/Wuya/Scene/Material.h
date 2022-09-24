#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/*
	 * 材质类
	 */
	class Material
	{
	public:
		Material() = default;
		~Material();

		/* 设置Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* 设置参数 */
		void SetParameters(const std::string& name, const std::any& param);
		/* 设置纹理 */
		void SetTexture(const SharedPtr<Texture>& texture, uint32_t slot);
		/* 设置光栅化状态 */
		void SetRasterState(RenderRasterState state) { m_RasterState = state; }
		RenderRasterState GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* 绑定材质中的各参数 */
		void Bind();
		/* 解绑材质 */
		void Unbind();

		/* 默认材质 */
		static SharedPtr<Material>& Default();
		/* 错误材质 */
		static SharedPtr<Material>& Error();

		/* 创建材质 */
		static SharedPtr<Material> Create(const SharedPtr<Shader>& shader);

	private:
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* 材质所需的各种参数<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* 材质所需的各种纹理<Texture，Slot> */
		std::unordered_map<SharedPtr<Texture>, uint32_t> m_Textures{};
		/* 光栅化状态配置 */
		RenderRasterState m_RasterState{};

		/* 默认材质和错误材质 */
		static SharedPtr<Material> m_pDefaultMaterial;
		static SharedPtr<Material> m_pErrorMaterial;
	};

	/* 材质组类：
	 * 每个模型对应一个MaterialGroup
	 * 每个模型中的MeshSegment对应MaterialGroup中的一个Material
	 */
	class MaterialGroup final
	{
	public:
		MaterialGroup(std::string path);
		~MaterialGroup() = default;

		/* 添加一个Material */
		void EmplaceMaterial(const SharedPtr<Material>& material);
		/* 根据索引获取Material */
		const SharedPtr<Material>& GetMaterialByIndex(int idx);
		/* 获取组中所有材质 */
		const std::vector<SharedPtr<Material>>& GetAllMaterials() const { return m_Materials; }

	private:
		/* 反序列化 */
		bool Deserializer();

		/* 材质组路径 */
		std::string m_Path{};
		/* 材质列表 */
		std::vector<SharedPtr<Material>> m_Materials;
	};
}
