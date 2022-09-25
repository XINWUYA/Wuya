#include "Pch.h"
#include "RenderView.h"
#include "FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Scene.h"
#include "Wuya/Scene/Components.h"
#include "Wuya/Scene/Material.h"

namespace Wuya
{
	RenderView::RenderView(std::string name)
		: m_DebugName(std::move(name))
	{
		PROFILE_FUNCTION();

		m_pFrameGraph = CreateSharedPtr<FrameGraph>(m_DebugName + "_FrameGraph");
	}

	RenderView::~RenderView()
	{
		PROFILE_FUNCTION();

		m_VisibleMeshObjects.clear();
	}

	/* �����ӿ����� */
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

	/* �洢��Pass��FrameBuffer */
	void RenderView::EmplacePassFrameBuffer(const std::string& name, const SharedPtr<FrameBuffer>& frame_buffer)
	{
		m_PassFrameBuffers[name] = frame_buffer;
	}

	const SharedPtr<FrameBuffer>& RenderView::GetPassFrameBuffer(const std::string& name) const
	{
		const auto it = m_PassFrameBuffers.find(name);
		if (it != m_PassFrameBuffers.end())
			return it->second;
		return {};
	}

	/* ׼��һ֡��RenderView���� */
	void RenderView::Prepare()
	{
		PROFILE_FUNCTION();

		/* ���ɵ�ǰFrameBuffer */
		m_pFrameGraph->Build();
	}

	/* ִ����Ⱦ��ǰView */
	void RenderView::Execute()
	{
		PROFILE_FUNCTION();

		m_VisibleMeshObjects.clear();

		/* �ռ���ǰRenderView�ɼ��Ķ��� */
		PrepareVisibleObjects();

		m_pFrameGraph->Execute();
	}

	/* ��׶���޳� */
	void RenderView::PrepareVisibleObjects()
	{
		PROFILE_FUNCTION();

		// todo: ��׶���޳�

		/* �ռ�����ģ�� */
		const auto owner_scene = m_pOwnerScene.lock();
		if (!owner_scene)
			return;

		const auto model_entity_group = owner_scene->GetRegistry().group<TransformComponent>(entt::get<ModelComponent>);
		for (auto& entity : model_entity_group)
		{
			auto [transform_component, model_component] = model_entity_group.get<TransformComponent, ModelComponent>(entity);

			for (const auto& mesh_segment : model_component.Model->GetMeshSegments())
			{
				const auto& world_position = transform_component.Position;
				// todo: ִ���޳�
				//if ()
				m_VisibleMeshObjects.emplace_back((int)entity, transform_component.GetTransform(), mesh_segment);
			}
		}

	}
}
