#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	/* 实体名称组件 */
	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(std::string name)
			: Name(std::move(name))
		{}
	};

	/* 空间变换组件 */
	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation)
		{}

		glm::mat4 GetTransform() const
		{
			const glm::mat4 rotation_mat = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation_mat
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	/* 场景相机组件 */
	struct CameraComponent
	{
		SceneCamera Camera{};
		bool IsPrimary{ true };
		bool IsFixedAspectRatio{ false };

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	/* 图片精灵组件 */
	struct SpriteComponent
	{
		SharedPtr<Texture2D> Texture;
		glm::vec4 BaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor{ 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& basecolor)
			: BaseColor(basecolor)
		{}
	};
}
