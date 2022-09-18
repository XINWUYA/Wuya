#pragma once
#include <glm/glm.hpp>
#include "Material.h"

namespace Wuya
{
	class MeshSegment;

	/* �ڽ�ģ������ */
	enum class BuiltinModelType : uint8_t
	{
		Cube,
		Sphere,
		Plane
	};


	/* ģ����
	 * Ĭ��Ϊ��̬ģ��
	 */
	class Model
	{
	public:
		Model(std::string name);
		virtual ~Model() = default;

		/* ����Ƿ�Ϊ��̬ģ�� */
		[[nodiscard]] virtual bool IsStaticModel() const { return true; }

		/* ģ��·�� */
		void SetPath(const std::string& path) { m_Path = path; }
		const std::string& GetPath() const { return m_Path; }

		/* ģ���� */
		const std::string& GetName() const { return m_DebugName; }

		/* ���һ��MeshSegment��ģ�� */
		void AddMeshSegment(const SharedPtr<MeshSegment>& mesh_segment);
		/* ��ȡ����MeshSegments */
		const std::vector<SharedPtr<MeshSegment>>& GetMeshSegments() const { return m_MeshSegments; }

		/* ��·������һ��ģ�� */
		static SharedPtr<Model> Create(const std::string& path);
		/* �����ڽ�ģ�� */
		static SharedPtr<Model> Create(BuiltinModelType type, const SharedPtr<Material>& material = Material::Default());

	private:
		/* ����� */
		std::string m_DebugName{ "Unnamed Model" };
		/* �ļ�·�� */
		std::string m_Path{};
		/* һ��ģ���а�������ģ�� */
		std::vector<SharedPtr<MeshSegment>> m_MeshSegments{};
		/* AABB */
		glm::vec3 m_AABBMin;
		glm::vec3 m_AABBMax;

		friend class SkeletonModel;
	};

	/* ����ģ���� */
	class SkeletonModel final : public Model
	{
	public:
		explicit SkeletonModel(const std::string& name);
		~SkeletonModel() override = default;

		/* ����Ƿ�Ϊ��̬ģ�� */
		[[nodiscard]] bool IsStaticModel() const override { return false; }

		/* ��·������һ������ģ�� */
		static SharedPtr<SkeletonModel> Create(const std::string& path);
	};
}
