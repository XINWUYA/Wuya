#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class Material;
	class VertexArray;

	/* ��Χ�� */
	struct AABB
	{
	};

	/* ����������
	 * ������ģ�ͣ�ÿ��ģ�Ϳ��ܶ�Ӧ���MeshSegments
	 * ÿ��MeshSegment���ж�����VertexArray��Material
	 */
	class MeshSegment
	{
	public:
		MeshSegment(std::string name);
		MeshSegment(std::string name, const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* ���� */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* ���ö������� */
		void SetVertexArray(const SharedPtr<VertexArray>& vertex_array) { m_pVertexArray = vertex_array; }
		[[nodiscard]] const SharedPtr<VertexArray>& GetVertexArray() const { return m_pVertexArray; }
		/* ���ò��� */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		[[nodiscard]] const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }
		/* ����AABB */
		[[nodiscard]] const glm::vec3& GetAABBMin() const { return m_AABBMin; }
		[[nodiscard]] const glm::vec3& GetAABBMax() const { return m_AABBMax; }
		void SetAABB(const glm::vec3& min, const glm::vec3& max) { m_AABBMin = min; m_AABBMax = max; }

	private:
		/* ����� */
		std::string m_DebugName{ "Unnamed MeshSegment" };
		/* �������� */
		SharedPtr<VertexArray> m_pVertexArray{ nullptr };
		/* ���� */
		SharedPtr<Material> m_pMaterial{ nullptr };
		/* AABB */
		glm::vec3 m_AABBMin{};
		glm::vec3 m_AABBMax{};

		friend class Model;
	};
}
