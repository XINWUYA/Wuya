#pragma once
#include <glm/glm.hpp>

#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Material;
	class VertexArray;

	/* ͼԪ */
	struct MeshPrimitive
	{
		PrimitiveType PrimitiveType{ PrimitiveType::Triangles };
		SharedPtr<VertexArray> VertexArray{ nullptr };

		MeshPrimitive(const SharedPtr<class VertexArray>& vertex_array, enum PrimitiveType type = PrimitiveType::Triangles)
			: PrimitiveType(type), VertexArray(vertex_array)
		{}
	};

	/* ����������
	 * ������ģ�ͣ�ÿ��ģ�Ϳ��ܶ�Ӧ���MeshSegments
	 * ÿ��MeshSegment���ж�����VertexArray��Material
	 */
	class MeshSegment
	{
	public:
		MeshSegment(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* ���� */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* ����ͼԪ */
		void SetVertexArray(const MeshPrimitive& mesh_primitive) { m_MeshPrimitive = mesh_primitive; }
		[[nodiscard]] const MeshPrimitive& GetMeshPrimitive() const { return m_MeshPrimitive; }
		/* ���ò��� */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		[[nodiscard]] const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }
		/* ����AABB */
		[[nodiscard]] const glm::vec3& GetAABBMin() const { return m_AABBMin; }
		[[nodiscard]] const glm::vec3& GetAABBMax() const { return m_AABBMax; }
		void SetAABB(const glm::vec3& min, const glm::vec3& max) { m_AABBMin = min; m_AABBMax = max; }

		/* �����������ݶ��� */
		static SharedPtr<MeshSegment> Create(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material);

	private:
		/* ����� */
		std::string m_DebugName{ "Unnamed MeshSegment" };
		/* ͼԪ���ͼ����� */
		MeshPrimitive m_MeshPrimitive;
		/* ���� */
		SharedPtr<Material> m_pMaterial{ nullptr };
		/* AABB */
		glm::vec3 m_AABBMin{};
		glm::vec3 m_AABBMax{};

		friend class Model;
	};
}
