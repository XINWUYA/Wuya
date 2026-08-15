#pragma once
#include <glm/glm.hpp>

namespace Helios
{
	enum class ObjectType : uint8_t
	{
		Invalid,
		Model,
		Light,
		Camera,
		Max
	};

	/* 场景对象基类
	 * 仅承载所有场景对象共有的空间信息：位置与旋转（欧拉角，弧度）。
	 * 缩放（Scale）是模型（Model）特有的属性，由 Model 自行管理，不放入基类。
	 */
	class SceneObject
	{
	public:
		SceneObject() = default;
		virtual ~SceneObject() {}

		/* 名字 */
		void SetDebugName(const std::string& name) { m_DebugName = name; }
		const std::string& GetDebugName() const { return m_DebugName; }

        /* 是否启用 */
        void SetEnable(bool enable) { m_Enable = enable; }
        bool GetEnable() const { return m_Enable; }

		/* 位置 */
		virtual const glm::vec3& GetPosition() const { return m_Position; }
		virtual void SetPosition(const glm::vec3& position) { m_Position = position; }

		/* 旋转（欧拉角，弧度） */
		virtual glm::vec3 GetRotation() const { return m_Rotation; }
		virtual void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

		/* 由世界变换矩阵写入位置与旋转（默认实现：分解矩阵后写入基类字段）。
		 * 派生类（如 Camera / Model）可重写以桥接自身变换逻辑。 */
		virtual void SetTransform(const glm::mat4& transform)
		{
			m_Position = glm::vec3(transform[3]);

			/* 去除各轴缩放，得到纯旋转矩阵 */
			const glm::mat3 rotation = glm::mat3(transform);
			const glm::vec3 col0 = glm::vec3(rotation[0]);
			const glm::vec3 col1 = glm::vec3(rotation[1]);
			const glm::vec3 col2 = glm::vec3(rotation[2]);
			const glm::vec3 scale = glm::vec3(
				glm::length(col0), glm::length(col1), glm::length(col2));
			const glm::mat3 pure_rotation(
				scale.x > 0.0f ? col0 / scale.x : col0,
				scale.y > 0.0f ? col1 / scale.y : col1,
				scale.z > 0.0f ? col2 / scale.z : col2);

			m_Rotation = glm::eulerAngles(glm::quat(pure_rotation));
		}

		/* 由位置与旋转组合出世界变换矩阵（与 TransformComponent::GetTransform 一致） */
		virtual glm::mat4 GetTransform() const
		{
			const glm::mat4 rotation_mat = glm::toMat4(glm::quat(m_Rotation));
			return glm::translate(glm::mat4(1.0f), m_Position) * rotation_mat;
		}

	protected:
		std::string m_DebugName{ "Unnamed" };
        bool m_Enable{ true };
		ObjectType m_ObjectType{ ObjectType::Invalid };
		glm::vec3 m_Position{ 0.0f };
		glm::vec3 m_Rotation{ 0.0f }; /* 欧拉角，弧度 */
	};
}
