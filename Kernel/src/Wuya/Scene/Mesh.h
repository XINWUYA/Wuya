#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class Material;
	class VertexArray;

	/* 包围盒 */
	struct AABB
	{
	};

	/* 网格数据类
	 * 隶属于模型，每个模型可能对应多个MeshSegments
	 * 每个MeshSegment具有独立的VertexArray和Material
	 */
	class MeshSegment
	{
	public:
		MeshSegment(std::string name);
		MeshSegment(std::string name, const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* 名称 */
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* 设置顶点数据 */
		void SetVertexArray(const SharedPtr<VertexArray>& vertex_array) { m_pVertexArray = vertex_array; }
		[[nodiscard]] const SharedPtr<VertexArray>& GetVertexArray() const { return m_pVertexArray; }
		/* 设置材质 */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		[[nodiscard]] const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }
		/* 设置AABB */
		[[nodiscard]] const glm::vec3& GetAABBMin() const { return m_AABBMin; }
		[[nodiscard]] const glm::vec3& GetAABBMax() const { return m_AABBMax; }
		void SetAABB(const glm::vec3& min, const glm::vec3& max) { m_AABBMin = min; m_AABBMax = max; }

	private:
		/* 标记名 */
		std::string m_DebugName{ "Unnamed MeshSegment" };
		/* 顶点数据 */
		SharedPtr<VertexArray> m_pVertexArray{ nullptr };
		/* 材质 */
		SharedPtr<Material> m_pMaterial{ nullptr };
		/* AABB */
		glm::vec3 m_AABBMin{};
		glm::vec3 m_AABBMax{};

		friend class Model;
	};
}
