#pragma once
#include <glm/glm.hpp>
#include "Material.h"

namespace Wuya
{
	class MeshSegment;

	/* 内建模型类型 */
	enum class BuiltinModelType : uint8_t
	{
		Cube,
		Sphere,
		Plane
	};


	/* 模型类
	 * 默认为静态模型
	 */
	class Model
	{
	public:
		Model(std::string name);
		virtual ~Model() = default;

		/* 标记是否为静态模型 */
		[[nodiscard]] virtual bool IsStaticModel() const { return true; }

		/* 模型路径 */
		void SetPath(const std::string& path) { m_Path = path; }
		const std::string& GetPath() const { return m_Path; }

		/* 模型名 */
		const std::string& GetName() const { return m_DebugName; }

		/* 添加一个MeshSegment到模型 */
		void AddMeshSegment(const SharedPtr<MeshSegment>& mesh_segment);
		/* 获取所有MeshSegments */
		const std::vector<SharedPtr<MeshSegment>>& GetMeshSegments() const { return m_MeshSegments; }

		/* 从路径加载一个模型 */
		static SharedPtr<Model> Create(const std::string& path);
		/* 创建内建模型 */
		static SharedPtr<Model> Create(BuiltinModelType type, const SharedPtr<Material>& material = Material::Default());

	private:
		/* 标记名 */
		std::string m_DebugName{ "Unnamed Model" };
		/* 文件路径 */
		std::string m_Path{};
		/* 一个模型中包含的子模型 */
		std::vector<SharedPtr<MeshSegment>> m_MeshSegments{};
		/* AABB */
		glm::vec3 m_AABBMin;
		glm::vec3 m_AABBMax;

		friend class SkeletonModel;
	};

	/* 骨骼模型类 */
	class SkeletonModel final : public Model
	{
	public:
		explicit SkeletonModel(const std::string& name);
		~SkeletonModel() override = default;

		/* 标记是否为静态模型 */
		[[nodiscard]] bool IsStaticModel() const override { return false; }

		/* 从路径加载一个骨骼模型 */
		static SharedPtr<SkeletonModel> Create(const std::string& path);
	};
}
