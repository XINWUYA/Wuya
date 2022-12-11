#pragma once
#include <tiny_obj_loader.h>

namespace Wuya
{
	/* ģ�Ͳ��ʲ����������������ɫ�� */
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

	/* ��ģ����Ϣ */
	struct SubModelInfo
	{
		std::string Name;
		SharedPtr<VertexArray> VertexArray;
		std::vector<float> VertexData; /* �������ݵ�ԭʼ��Ϣ������ģ��ʱ��Ҫ */
		MaterialParams MaterialParams;
		std::pair<glm::vec3, glm::vec3> AABB;
	};

	/* ����ģ��ԭʼ�ļ���Obj�� */
	class ModelInfo final
	{
	public:
		~ModelInfo() = default;

		/* ����Objģ����Ϣ */
		void LoadFromObj(const std::string& filepath);

		/* ������Mesh */
		void ExportMesh(const std::string& filepath);

		/* �������� */
		void Reset();

	private:
		/* ����·�� */
		std::string m_Path{};
		/* �������е�vertex��normal��texcoords */
		tinyobj::attrib_t m_Attributes;
		/* ����ÿ���������ݶε�indices��faces��material_idx */
		std::vector<tinyobj::shape_t> m_Shapes;
		/* ��������textures����diffuse, specular, normal�� */
		std::vector<tinyobj::material_t> m_Materials;
		/* ģ�Ͱ�������ģ������ */
		std::vector<SharedPtr<SubModelInfo>> m_SubModelInfos;
		/* �����AABB */
		std::pair<glm::vec3, glm::vec3> m_AABB;

		friend class ModelEditorLayer;
	};


}
