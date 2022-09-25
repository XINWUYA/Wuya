#include "Pch.h"
#include "Scene.h"
#include <tinyxml2.h>
#include "Components.h"
#include "Entity.h"
#include "Material.h"
#include "Wuya/Common/Utils.h"
#include "Wuya/Renderer/Camera.h"
#include "Wuya/Renderer/Renderer.h"
#include "Wuya/Renderer/Renderer2D.h"
#include "Wuya/Renderer/RenderView.h"

namespace Wuya
{
	Scene::~Scene()
	{
		m_RenderViews.clear();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		PROFILE_FUNCTION();

		Entity entity = { m_Registry.create(), shared_from_this() };

		/* 默认添加名称组件和变换组件 */
		auto& name_component = entity.AddComponent<NameComponent>();
		name_component.Name = name.empty() ? "Unnamed Entity" : name;

		entity.AddComponent<TransformComponent>();

		return entity;
	}

	void Scene::DestroyEntity(Entity& entity)
	{
		PROFILE_FUNCTION();

		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateRuntime(float delta_time)
	{
		PROFILE_FUNCTION();

		/* 更新所有实体的变换 */
		// auto entity_group = m_Registry.group<TransformComponent>(entt::get<>)
	}

	void Scene::OnUpdateEditor(const SharedPtr<Camera>& camera, float delta_time)
	{
		PROFILE_FUNCTION();

		/* todo: 收集RenderView */
		m_RenderViews.clear();
		const auto& entity_view = GetRegistry().group<CameraComponent>(entt::get<NameComponent>);
		for (auto& entity : entity_view)
		{
			const auto [name_component, camera_component] = entity_view.get<NameComponent, CameraComponent>(entity);
			SharedPtr<RenderView> render_view = CreateSharedPtr<RenderView>(name_component.Name + "_RenderView");
			//render_view->SetName(name_component.Name);
			render_view->SetCullingCamera(camera);// todo: 替换为组件对应的相机
			m_RenderViews.emplace_back(render_view);
		}

		/* Editor Camera's RenderView, 最后一个是编辑器RenderView */
		if (camera)
		{
			auto& render_view = camera->GetRenderView();
			render_view->SetOwnerScene(shared_from_this());
			//render_view->SetViewportRegion(camera->)
			m_RenderViews.emplace_back(render_view);
		}

		/* 绘制所有View */
		for (const auto& view : m_RenderViews)
		{
			Renderer::RenderAView(view);
		}


		/* 更新Renderer2D信息 */
		Renderer2D::BeginFrame(camera);

		/* 更新所有图片实体 */
		/*const auto& sprite_entity_group = GetRegistry().group<TransformComponent>(entt::get<SpriteComponent>);
		for (auto& entity : sprite_entity_group)
		{
			auto [transform_component, sprite_component] = sprite_entity_group.get<TransformComponent, SpriteComponent>(entity);
			Renderer2D::DrawSprite(transform_component.GetTransform(), &sprite_component, static_cast<int>(entity));
		}*/

		/* 提交绘制数据 */
		Renderer2D::EndFrame();
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		PROFILE_FUNCTION();

		/* difference of view and group: https://github.com/skypjack/entt/discussions/638 */
		const auto entity_view = m_Registry.view<CameraComponent>();
		for (auto& entity : entity_view)
		{
			const auto& camera_component = entity_view.get<CameraComponent>(entity);
			if (camera_component.IsPrimary)
				return Entity{ entity, shared_from_this() };
		}
		return {};
	}

	/* 获取主相机的RenderTarget Texture */
	const SharedPtr<Texture>& Scene::GetPrimaryCameraRenderTargetTexture() const
	{
		PROFILE_FUNCTION();

		auto& render_view = m_RenderViews.back();
		return render_view->GetRenderTarget();
	}

	void Scene::Serializer(const std::string& path)
	{
		PROFILE_FUNCTION();

		auto* doc = new tinyxml2::XMLDocument();
		doc->InsertEndChild(doc->NewDeclaration());
		tinyxml2::XMLElement* scene_root = doc->NewElement("Scene");
		doc->InsertEndChild(scene_root);

		tinyxml2::XMLElement* entities_root = scene_root->InsertNewChildElement("Entities");

		/* 遍历场景中实体，进行序列化 */
		m_Registry.each(
			[&](auto& entity_id)
			{
				Entity entity = { entity_id, shared_from_this() };
				if (!entity)
					return;

				SerializeEntity(entities_root, entity);
			}
		);

		/* 保存到文本 */
		doc->SaveFile(path.c_str());

		delete doc;
	}

	bool Scene::Deserializer(const std::string& path)
	{
		PROFILE_FUNCTION();

		auto* doc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError error = doc->LoadFile(path.c_str());
		if (error != tinyxml2::XML_SUCCESS)
		{
			CORE_LOG_ERROR("Failed to deserializer scene file: {}.", path);
			return false;
		}

		tinyxml2::XMLElement* scene_root = doc->FirstChildElement("Scene");
		tinyxml2::XMLElement* entities_root = scene_root->FirstChildElement("Entities");

		for (tinyxml2::XMLElement* entity_root = entities_root->FirstChildElement(); entity_root; entity_root = entity_root->NextSiblingElement("Entity"))
		{
			if (auto* id_attr = entity_root->FindAttribute("ID"))
			{
				int entity_id = id_attr->IntValue();

				/* Name */
				std::string name;
				if (auto* name_attr = entity_root->FindAttribute("Name"))
					name = name_attr->Value();

				if (!name.empty())
				{
					auto entity = CreateEntity(name);

					/* Transform */
					if (const auto* transform_root = entity_root->FirstChildElement("Transform"))
					{
						auto& transform_component = entity.GetComponent<TransformComponent>();
						transform_component.Position = ToVec3(transform_root->Attribute("Position"));
						transform_component.Rotation = ToVec3(transform_root->Attribute("Rotation"));
						transform_component.Scale = ToVec3(transform_root->Attribute("Scale"));
					}

					/* Sprite */
					if (const auto* sprite_root = entity_root->FirstChildElement("Sprite"))
					{
						auto& sprite_component = entity.AddComponent<SpriteComponent>();
						const std::string texture_path = sprite_root->Attribute("TexturePath");
						sprite_component.Texture = Texture::Create(texture_path);
						sprite_component.BaseColor = ToVec4(sprite_root->Attribute("BaseColor"));
						sprite_component.TilingFactor = sprite_root->FloatAttribute("TilingFactor");
					}

					/* Camera */
					if (const auto* camera_root = entity_root->FirstChildElement("Camera"))
					{
						auto& camera_component = entity.AddComponent<CameraComponent>();
						camera_component.IsPrimary = camera_root->BoolAttribute("IsPrimary");
						camera_component.IsFixedAspectRatio = camera_root->BoolAttribute("IsFixedAspectRatio");

						const auto projection_type = static_cast<SceneCamera::ProjectionType>(camera_root->IntAttribute("ProjectionType"));
						camera_component.Camera.SetProjectionType(projection_type);
						switch (projection_type)
						{
						case SceneCamera::ProjectionType::Perspective:
							{
								const float fov = camera_root->FloatAttribute("Fov");
								const float near_clip = camera_root->FloatAttribute("Near");
								const float far_clip = camera_root->FloatAttribute("Far");
								camera_component.Camera.SetPerspectiveCameraDesc({ fov, near_clip, far_clip });
							}
							break;
						case SceneCamera::ProjectionType::Orthographic:
							{
								const float height_size = camera_root->FloatAttribute("HeightSize");
								const float near_clip = camera_root->FloatAttribute("Near");
								const float far_clip = camera_root->FloatAttribute("Far");
								camera_component.Camera.SetOrthographicCameraDesc({ height_size, near_clip, far_clip });
							}
							break;
						}
					}

					/* Model */
					if (auto* model_root = entity_root->FirstChildElement("Model"))
					{
						auto& model_component = entity.AddComponent<ModelComponent>();

						const std::string model_path = model_root->Attribute("ModelPath");

						if (!model_path.empty())
							model_component.Model = Model::Create(model_path);
						

						/* todo: 内建模型处理 */

					}

				}
			}	
		}

		delete doc;
		return true;
	}

	void Scene::SerializeEntity(tinyxml2::XMLElement* root_node, Entity& entity)
	{
		PROFILE_FUNCTION();

		tinyxml2::XMLElement* entity_root = root_node->InsertNewChildElement("Entity");

		/* ID */
		entity_root->SetAttribute("ID", (int)entity);

		/* Name */
		if (entity.HasComponent<NameComponent>())
		{
			const auto& component = entity.GetComponent<NameComponent>();
			entity_root->SetAttribute("Name", component.Name.c_str());
		}

		/* Transform */
		if (entity.HasComponent<TransformComponent>())
		{
			auto* transform_root = entity_root->InsertNewChildElement("Transform");
			const auto& component = entity.GetComponent<TransformComponent>();
			transform_root->SetAttribute("Position", ToString(component.Position).c_str());
			transform_root->SetAttribute("Rotation", ToString(component.Rotation).c_str());
			transform_root->SetAttribute("Scale", ToString(component.Scale).c_str());
		}

		/* Sprite */
		if (entity.HasComponent<SpriteComponent>())
		{
			auto* sprite_root = entity_root->InsertNewChildElement("Sprite");
			const auto& component = entity.GetComponent<SpriteComponent>();
			sprite_root->SetAttribute("TexturePath", component.Texture->GetPath().c_str());
			sprite_root->SetAttribute("BaseColor", ToString(component.BaseColor).c_str());
			sprite_root->SetAttribute("TilingFactor", component.TilingFactor);
		}

		/* Camera */
		if (entity.HasComponent<CameraComponent>())
		{
			auto* camera_root = entity_root->InsertNewChildElement("Camera");
			const auto& component = entity.GetComponent<CameraComponent>();

			camera_root->SetAttribute("IsPrimary", component.IsPrimary);
			camera_root->SetAttribute("IsFixedAspectRatio", component.IsFixedAspectRatio);

			const auto projection_type = component.Camera.GetProjectionType();
			camera_root->SetAttribute("ProjectionType", static_cast<int>(projection_type));

			switch (projection_type)
			{
			case SceneCamera::ProjectionType::Perspective:
				{
					const auto& camera_desc = component.Camera.GetPerspectiveCameraDesc();
					camera_root->SetAttribute("Fov", camera_desc->Fov);
					camera_root->SetAttribute("Near", camera_desc->Near);
					camera_root->SetAttribute("Far", camera_desc->Far);
				}
				break;
			case SceneCamera::ProjectionType::Orthographic:
				{
				const auto& camera_desc = component.Camera.GetOrthographicCameraDesc();
				camera_root->SetAttribute("HeightSize", camera_desc->HeightSize);
				camera_root->SetAttribute("Near", camera_desc->Near);
				camera_root->SetAttribute("Far", camera_desc->Far);
				}
				break;
			}
			
		}

		/* Model */
		if (entity.HasComponent<ModelComponent>())
		{
			auto* model_root = entity_root->InsertNewChildElement("Model");
			const auto& component = entity.GetComponent<ModelComponent>();

			model_root->SetAttribute("ModelPath", component.Model->GetPath().c_str());
		}
	}

	template <typename T>
	void Scene::OnComponentAdded(Entity& entity, T& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NameComponent>(Entity& entity, NameComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity& entity, TransformComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<SpriteComponent>(Entity& entity, SpriteComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity& entity, CameraComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<ModelComponent>(Entity& entity, ModelComponent& component)
	{

	}
}
