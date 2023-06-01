#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "SceneCamera.h"
#include "Wuya/Renderer/Texture.h"
#include "Model.h"
#include "Light.h"

namespace Wuya
{
	/* ʵ��������� */
	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(std::string name)
			: Name(std::move(name))
		{}
	};

	/* �ռ�任��� */
	struct TransformComponent
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position)
			: Position(position)
		{}

		glm::mat4 GetTransform() const
		{
			const glm::mat4 rotation_mat = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Position)
				* rotation_mat
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	/* ���������� */
	struct CameraComponent
	{
		SharedPtr<SceneCamera> Camera{ nullptr };
		bool IsPrimary{ true };
		bool IsFixedAspectRatio{ false };

		CameraComponent(const CameraComponent&) = default;
		CameraComponent()
		{
			Camera = CreateSharedPtr<SceneCamera>();
		}

	};

	/* ͼƬ������� */
	struct SpriteComponent
	{
		SharedPtr<Texture> Texture{ nullptr };
		glm::vec4 BaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		float TilingFactor{ 1.0f };

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& basecolor)
			: BaseColor(basecolor)
		{}
	};

	/* ģ����� */
	struct ModelComponent
	{
		SharedPtr<Model> Model{ nullptr };

		ModelComponent() = default;
		ModelComponent(const ModelComponent&) = default;
	};

	/* ��Դ��� */
	struct LightComponent
	{
		SharedPtr<Light> Light{ nullptr };
		LightType Type{ LightType::Point };

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(LightType type)
			: Type(type)
		{
			Light = Light::Create(type);
		}
	};
}
