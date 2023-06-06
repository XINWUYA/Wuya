#pragma once
#include <tiny_obj_loader.h>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Wuya
{
	/* 模型材质参数，包括纹理和颜色等 */
	struct MaterialParams
	{
		/* Name */
		std::string Name;

		/* Ambient */
		std::pair<std::string, bool> AmbientTexPath{"", false};
		std::pair<glm::vec3, bool> AmbientFactor;

		/* Diffuse/Albedo */
		std::pair<std::string, bool> DiffuseTexPath;
		std::pair<glm::vec3, bool> DiffuseFactor;

		/* Specular */
		std::pair<std::string, bool> SpecularTexPath;
		std::pair<glm::vec3, bool> SpecularFactor;

		/* Normal */
		std::pair<std::string, bool> NormalTexPath;
		std::pair<std::string, bool> BumpTexPath;
		std::pair<std::string, bool> DisplacementTexPath;

		/* Roughness */
		std::pair<std::string, bool> RoughnessTexPath;
		std::pair<float, bool> RoughnessFactor;

		/* Metallic */
		std::pair<std::string, bool> MetallicTexPath;
		std::pair<float, bool> MetallicFactor;

		/* Emission */
		std::pair<std::string, bool> EmissionTexPath;
		std::pair<glm::vec3, bool> EmissionFactor;

		/* ClearCoat */
		std::pair<float, bool> ClearCoatRoughness;
		std::pair<float, bool> ClearCoatThickness;

		/* Others */
		std::pair<glm::vec3, bool> TransmittanceColor;
		std::pair<float, bool> IOR;
	};

	/* 子模型信息 */
	struct SubModelInfo
	{
		std::string Name;
		SharedPtr<VertexArray> VertexArray;
		uint32_t VertexCount;
		std::vector<std::pair<uint32_t, float*>> VertexBufferDatas; /* 顶点数据的原始信息，保存模型时需要<stride, data> */
		std::vector<uint32_t> Indices; /* 顶点数据的索引信息 */
		MaterialParams MaterialParams;
		std::pair<glm::vec3, glm::vec3> AABB;

		~SubModelInfo();
	};

	/* 加载模型原始文件（Obj） */
	class ModelInfo final
	{
	public:
		~ModelInfo() = default;

		/* 加载Obj模型信息 */
		void LoadFromObj(const std::string& filepath);

		/* 使用assimp */
		void LoadFromPath(const std::string& filepath);

		/* 重置数据 */
		void Reset();

	private:
		void LoadNode(const aiNode* node, const aiScene* scene);
		void LoadMesh(const aiMesh* mesh, const aiScene* scene);

		/* 所在路径 */
		std::string m_Path{};
		/* 包含所有的vertex、normal、texcoords */
		tinyobj::attrib_t m_Attributes;
		/* 包含每个网格数据段的indices、faces、material_idx */
		std::vector<tinyobj::shape_t> m_Shapes;
		/* 包含各种textures，如diffuse, specular, normal等 */
		std::vector<tinyobj::material_t> m_Materials;
		/* 模型包含的子模型数据 */
		std::vector<SharedPtr<SubModelInfo>> m_SubModelInfos{};
		/* 整体的AABB */
		std::pair<glm::vec3, glm::vec3> m_AABB{};

		/* 模型所在的根目录 */
		std::string m_Directory{};

		friend class ModelEditor;
	};


}
