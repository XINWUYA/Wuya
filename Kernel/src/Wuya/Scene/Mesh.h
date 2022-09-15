#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class Material;
	class VertexArray;

	/* ��Χ�� */
	struct AABB
	{
		glm::vec3 Min;
		glm::vec3 Max;
	};

	/* �������� */
	class MeshSegment
	{
	public:
		MeshSegment(const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* ���ö������� */
		void SetVertexArray(const SharedPtr<VertexArray>& vertex_array) { m_pVertexArray = vertex_array; }
		const SharedPtr<VertexArray>& GetVertexArray() const { return m_pVertexArray; }
		/* ���ò��� */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }

	private:
		/* �������� */
		SharedPtr<VertexArray> m_pVertexArray{ nullptr };
		/* ���� */
		SharedPtr<Material> m_pMaterial{ nullptr };

		/* AABB */
		AABB m_AABB{};
	};


	/* ͼԪ���� */
	enum class PrimitiveType : uint8_t
	{
		Cube,
		Sphere,
		Plane
	};

	/* �������ʹ���һ���������� */
	SharedPtr<MeshSegment> CreatePrimitive(PrimitiveType type, const SharedPtr<Material>& material);

	/* ���ļ�����һ��Objģ�� */
	std::vector<SharedPtr<MeshSegment>> LoadObjMeshFromFile(const std::string& filepath);
}
