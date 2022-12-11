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
		std::vector<float> VertexData; /* 顶点数据的原始信息，保存模型时需要 */
		MaterialParams MaterialParams;
		std::pair<glm::vec3, glm::vec3> AABB;
	};

	/* 加载模型原始文件（Obj） */
	class ModelInfo final
	{
	public:
		~ModelInfo() = default;

		/* 加载Obj模型信息 */
		void LoadFromObj(const std::string& filepath);

		/* 导出成Mesh */
		void ExportMesh(const std::string& filepath);

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
		std::vector<SharedPtr<SubModelInfo>> m_SubModelInfos;
		/* 整体的AABB */
		std::pair<glm::vec3, glm::vec3> m_AABB;

		friend class ModelEditorLayer;
	};


}
