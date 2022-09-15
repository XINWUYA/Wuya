#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class Material;
	class VertexArray;

	/* 包围盒 */
	struct AABB
	{
		glm::vec3 Min;
		glm::vec3 Max;
	};

	/* 网格数据 */
	class MeshSegment
	{
	public:
		MeshSegment(const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material);
		~MeshSegment() = default;

		/* 设置顶点数据 */
		void SetVertexArray(const SharedPtr<VertexArray>& vertex_array) { m_pVertexArray = vertex_array; }
		const SharedPtr<VertexArray>& GetVertexArray() const { return m_pVertexArray; }
		/* 设置材质 */
		void SetMaterial(const SharedPtr<Material>& material) { m_pMaterial = material; }
		const SharedPtr<Material>& GetMaterial() const { return m_pMaterial; }

	private:
		/* 顶点数据 */
		SharedPtr<VertexArray> m_pVertexArray{ nullptr };
		/* 材质 */
		SharedPtr<Material> m_pMaterial{ nullptr };

		/* AABB */
		AABB m_AABB{};
	};


	/* 图元类型 */
	enum class PrimitiveType : uint8_t
	{
		Cube,
		Sphere,
		Plane
	};

	/* 根据类型创建一个常规网格 */
	SharedPtr<MeshSegment> CreatePrimitive(PrimitiveType type, const SharedPtr<Material>& material);

	/* 从文件加载一个Obj模型 */
	std::vector<SharedPtr<MeshSegment>> LoadObjMeshFromFile(const std::string& filepath);
}
