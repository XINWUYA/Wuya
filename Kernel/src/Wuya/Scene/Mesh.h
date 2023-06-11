#pragma once
#include <glm/glm.hpp>

#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Material;
	class VertexArray;

	/* 图元 */
	struct MeshPrimitive
	{
		PrimitiveType PrimitiveType{ PrimitiveType::Triangles };
		SharedPtr<VertexArray> VertexArray{ nullptr };

		MeshPrimitive(const SharedPtr<class VertexArray>& vertex_array, enum PrimitiveType type = PrimitiveType::Triangles)
			: PrimitiveType(type), VertexArray(vertex_array)
		{}
	};

	/* 网格数据类
	 * 隶属于模型，每个模型可能对应多个MeshSegments
	 * 每个MeshSegment具有独立的VertexArray和Material
	 */
	class MeshSegment
	{
	public:
		MeshSegment(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* 名称 */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* 设置图元 */
		void SetVertexArray(const MeshPrimitive& mesh_primitive) { m_MeshPrimitive = mesh_primitive; }
		[[nodiscard]] const MeshPrimitive& GetMeshPrimitive() const { return m_MeshPrimitive; }
		/* 设置材质 */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		[[nodiscard]] const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }
		/* 设置AABB */
		[[nodiscard]] const glm::vec3& GetAABBMin() const { return m_AABBMin; }
		[[nodiscard]] const glm::vec3& GetAABBMax() const { return m_AABBMax; }
		void SetAABB(const glm::vec3& min, const glm::vec3& max) { m_AABBMin = min; m_AABBMax = max; }

		/* 创建网格数据对象 */
		static SharedPtr<MeshSegment> Create(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material);

	private:
		/* 标记名 */
		std::string m_DebugName{ "Unnamed MeshSegment" };
		/* 图元类型及顶点 */
		MeshPrimitive m_MeshPrimitive;
		/* 材质 */
		SharedPtr<Material> m_pMaterial{ nullptr };
		/* AABB */
		glm::vec3 m_AABBMin{};
		glm::vec3 m_AABBMax{};

		friend class Model;
	};
}
