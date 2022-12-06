#pragma once
#include <tiny_obj_loader.h>

namespace Wuya
{
	/* 模型材质参数，包括纹理和颜色等 */
	struct MaterialParams
	{
		/* Ambient */
		std::string AmbientTexPath;
		glm::vec3 Ambient;

		/* Diffuse/Albedo */
		std::string DiffuseTexPath;
		glm::vec3 Diffuse;

		/* Specular */
		std::string SpecularTexPath;
		glm::vec3 Specular;

		/* Normal */
		std::string BumpTexPath;
		std::string DisplacementTexPath;

		/* Roughness */
		std::string RoughnessTexPath;
		float Roughness;

		/* Metallic */
		std::string MetallicTexPath;
		float Metallic;

		/* Emission */
		std::string EmissionTexPath;
		glm::vec3 Emission;

		/* ClearCoat */
		float ClearCoatRoughness;
		float ClearCoatThickness;

		/* Others */
		glm::vec3 Transmittance;
		float IOR;
	};

	/* 子模型信息 */
	struct SubModelInfo
	{
		std::string Name;
		SharedPtr<VertexArray> VertexArray;
		MaterialParams MaterialParams;
		std::pair<glm::vec3, glm::vec3> AABB;

		SubModelInfo(std::string name, const SharedPtr<class VertexArray>& vertex_array, struct MaterialParams params, std::pair<glm::vec3, glm::vec3> aabb)
			: Name(std::move(name)), VertexArray(vertex_array), MaterialParams(std::move(params)), AABB(std::move(aabb))
		{}
	};

	/* 加载模型原始文件（Obj） */
	class ModelInfo final
	{
	public:
		~ModelInfo() = default;

		/* 加载Obj模型信息 */
		void LoadFromObj(const std::string& filepath);

		/* 保存模型 */
		void Save(const std::string& filepath);

		/* 重置数据 */
		void Reset();

	private:
		/* 所在路径 */
		std::string m_Path{};
		/* 包含所有的vertex、normal、texcoords */
		tinyobj::attrib_t m_Attributes;
		/* 包含每个网格数据段的indices、faces、material_idx */
		std::vector<tinyobj::shape_t> m_Shapes;
		/* 包含各种textures，如diffuse, specular, normal等 */
		std::vector<tinyobj::material_t> m_Materials;
		/* 模型包含的子模型数据 */
		std::vector<SubModelInfo> m_SubModelInfos;
		/* 整体的AABB */
		std::pair<glm::vec3, glm::vec3> m_AABB;

		friend class ModelEditorLayer;
	};


}
