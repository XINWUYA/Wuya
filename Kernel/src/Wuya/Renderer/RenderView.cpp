#include "Pch.h"
#include "RenderView.h"
#include "FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Scene.h"
#include "Wuya/Scene/Components.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/ShadowMap.h"

namespace Wuya
{
	RenderView::RenderView(std::string name, Camera* owner_camera)
		: m_DebugName(std::move(name)), m_pOwnerCamera(owner_camera)
	{
		PROFILE_FUNCTION();

		m_pFrameGraph = CreateSharedPtr<FrameGraph>(m_DebugName + "_FrameGraph");
		m_pShadowMapManager = CreateSharedPtr<ShadowMapManager>();
	}

	RenderView::~RenderView()
	{
		PROFILE_FUNCTION();

		m_VisibleMeshObjects.clear();
	}

	/* 设置视口区域 */
	void RenderView::SetViewportRegion(const ViewportRegion& region)
	{
		PROFILE_FUNCTION();

		ASSERT(region.Width > 0 && region.Height > 0, "ViewprotRegion's width or height is invaild.");
		m_ViewportRegion = region;
	}

	SharedPtr<Texture> RenderView::GetRenderTarget() const
	{
		PROFILE_FUNCTION();

		if (!m_RenderTargetHandle.IsInitialized())
			return nullptr;

		return DynamicPtrCast<Resource<FrameGraphTexture>>(m_pFrameGraph->GetResource(m_RenderTargetHandle))->GetResource().Texture;
	}

	/* 存储各Pass的FrameBuffer */
	void RenderView::EmplacePassFrameBuffer(const std::string& name, const SharedPtr<FrameBuffer>& frame_buffer)
	{
		PROFILE_FUNCTION();

		m_PassFrameBuffers[name] = frame_buffer;
	}

	const SharedPtr<FrameBuffer>& RenderView::GetPassFrameBuffer(const std::string& name) const
	{
		PROFILE_FUNCTION();

		const auto it = m_PassFrameBuffers.find(name);
		if (it != m_PassFrameBuffers.end())
			return it->second;
		return {};
	}

	/* 准备一帧的RenderView数据 */
	void RenderView::Prepare()
	{
		PROFILE_FUNCTION();

		/* 生成当前FrameGraph */
		m_pFrameGraph->Build();
	}

	/* 执行渲染当前View */
	void RenderView::Execute()
	{
		PROFILE_FUNCTION();

		/* 收集当前RenderView可见的对象 */
		PrepareVisibleObjects();
		/* 收集光源信息 */
		PrepareLights();

		m_pFrameGraph->Execute();
	}

	/* 视锥体剔除 */
	void RenderView::PrepareVisibleObjects()
	{
		PROFILE_FUNCTION();

		m_VisibleMeshObjects.clear();

		// todo: 视锥体剔除

		/* 收集所有模型 */
		const auto owner_scene = m_pOwnerScene.lock();
		if (!owner_scene)
			return;

		const auto model_entity_view = owner_scene->GetRegistry().view<TransformComponent, ModelComponent>();
		for (auto& entity : model_entity_view)
		{
			auto [transform_component, model_component] = model_entity_view.get<TransformComponent, ModelComponent>(entity);
			if (!model_component.Model)
				 continue;

			for (const auto& mesh_segment : model_component.Model->GetMeshSegments())
			{
				const auto& world_position = transform_component.Position;
				// todo: 执行剔除
				//if ()
				m_VisibleMeshObjects.emplace_back((int)entity, transform_component.GetTransform(), mesh_segment);
			}
		}

	}

	/* 准备光源信息 */
	void RenderView::PrepareLights()
	{
		PROFILE_FUNCTION();

		m_ValidLights.clear();
		m_IsHasShadowCast = false;
		m_pShadowMapManager.reset();

		/* 收集所有光源 */
		const auto owner_scene = m_pOwnerScene.lock();
		if (!owner_scene)
			return;

		const auto light_entity_view = owner_scene->GetRegistry().view<TransformComponent, LightComponent>();
		for (auto& entity : light_entity_view)
		{
			auto [transform_component, light_component] = light_entity_view.get<TransformComponent, LightComponent>(entity);

			const glm::vec3 light_dir = transform_component.GetTransform() * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			const auto& light_color = light_component.Light->GetColor();
			const bool cast_shadow = light_component.Light->IsCastShadow();
			m_ValidLights.emplace_back(static_cast<uint32_t>(light_component.Type), 
				glm::vec4(light_color.r, light_color.g, light_color.b, light_component.Light->GetIntensity()), 
				light_dir, 
				transform_component.Position, 
				cast_shadow);

			if (cast_shadow)
			{
				m_pShadowMapManager->AddShadowMap(light_component.Light);
				m_IsHasShadowCast = true;
			}
		}

		if (m_IsHasShadowCast)
			m_pShadowMapManager->PrepareAllShadowMaps(owner_scene);
	}
}
