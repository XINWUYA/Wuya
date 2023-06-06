#pragma once
#include <tiny_obj_loader.h>

struct aiScene;
struct aiNode;
struct aiMesh;

namespace Wuya
{
	/* ģ�Ͳ��ʲ����������������ɫ�� */
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

	/* ��ģ����Ϣ */
	struct SubModelInfo
	{
		std::string Name;
		SharedPtr<VertexArray> VertexArray;
		uint32_t VertexCount;
		std::vector<std::pair<uint32_t, float*>> VertexBufferDatas; /* �������ݵ�ԭʼ��Ϣ������ģ��ʱ��Ҫ<stride, data> */
		std::vector<uint32_t> Indices; /* �������ݵ�������Ϣ */
		MaterialParams MaterialParams;
		std::pair<glm::vec3, glm::vec3> AABB;

		~SubModelInfo();
	};

	/* ����ģ��ԭʼ�ļ���Obj�� */
	class ModelInfo final
	{
	public:
		~ModelInfo() = default;

		/* ����Objģ����Ϣ */
		void LoadFromObj(const std::string& filepath);

		/* ʹ��assimp */
		void LoadFromPath(const std::string& filepath);

		/* �������� */
		void Reset();

	private:
		void LoadNode(const aiNode* node, const aiScene* scene);
		void LoadMesh(const aiMesh* mesh, const aiScene* scene);

		/* ����·�� */
		std::string m_Path{};
		/* �������е�vertex��normal��texcoords */
		tinyobj::attrib_t m_Attributes;
		/* ����ÿ���������ݶε�indices��faces��material_idx */
		std::vector<tinyobj::shape_t> m_Shapes;
		/* ��������textures����diffuse, specular, normal�� */
		std::vector<tinyobj::material_t> m_Materials;
		/* ģ�Ͱ�������ģ������ */
		std::vector<SharedPtr<SubModelInfo>> m_SubModelInfos{};
		/* �����AABB */
		std::pair<glm::vec3, glm::vec3> m_AABB{};

		/* ģ�����ڵĸ�Ŀ¼ */
		std::string m_Directory{};

		friend class ModelEditor;
	};


}
