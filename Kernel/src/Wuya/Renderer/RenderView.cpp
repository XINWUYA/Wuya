#include "Pch.h"
#include "RenderView.h"
#include "Wuya/Scene/Mesh.h"
#include "Wuya/Scene/Scene.h"
#include "Wuya/Scene/Components.h"
#include "Wuya/Scene/Material.h"

namespace Wuya
{
	RenderView::RenderView(const std::string& name, const SharedPtr<Camera>& culling_camera, bool enable_culling)
		: m_Name(name), m_pCullingCamera(culling_camera), m_IsEnableCulling(enable_culling)
	{
	}

	/* �����ӿ����� */
	void RenderView::SetViewportRegion(const ViewportRegion& region)
	{
		ASSERT(region.Width() > 0 && region.Height() > 0, "ViewprotRegion's width or height is invaild.");
		m_ViewportRegion = region;
	}

	/* ׼��һ֡��RenderView���� */
	void RenderView::Prepare()
	{
		/* �ռ���ǰRenderView�ɼ��Ķ��� */
		PrepareVisibleObjects();
	}

	/* ��׶���޳� */
	void RenderView::PrepareVisibleObjects()
	{
		// todo: ��׶���޳�

		/* �ռ�����ģ�� */
		const auto mesh_entity_group = m_pOwnerScene->GetRegistry().group<TransformComponent>(entt::get<MeshComponent>);
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
