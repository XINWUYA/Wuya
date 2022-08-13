#include "Pch.h"
#include "RenderView.h"
#include "FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Scene.h"
#include "Wuya/Scene/Components.h"
#include "Wuya/Scene/Material.h"

namespace Wuya
{
	RenderView::RenderView(const std::string& name)
		: m_DebugName(name)
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

	/* ׼��һ֡��RenderView���� */
	void RenderView::Prepare()
	{
		PROFILE_FUNCTION();

		m_VisibleMeshObjects.clear();

		/* �ռ���ǰRenderView�ɼ��Ķ��� */
		PrepareVisibleObjects();

		/* ���ɵ�ǰFrameBuffer */
		m_pFrameGraph->Build();
	}

	/* ִ����Ⱦ��ǰView */
	void RenderView::Execute()
	{
		PROFILE_FUNCTION();

		m_pFrameGraph->Execute();
	}

	/* ��׶���޳� */
	void RenderView::PrepareVisibleObjects()
	{
		PROFILE_FUNCTION();

		// todo: ��׶���޳�

		/* �ռ�����ģ�� */
		const auto owner_scene = m_pOwnerScene.lock();
		const auto mesh_entity_group = owner_scene->GetRegistry().group<TransformComponent>(entt::get<MeshComponent>);
		for (auto& entity : mesh_entity_group)
		{
			auto [transform_component, mesh_component] = mesh_entity_group.get<TransformComponent, MeshComponent>(entity);

			for (const auto& mesh_segment : mesh_component.MeshSegments)
			{
				const auto& world_position = transform_component.Position;
				// todo: ִ���޳�
				//if ()
				m_VisibleMeshObjects.emplace_back(transform_component.GetTransform(), mesh_segment);
			}
		}

	}
}
