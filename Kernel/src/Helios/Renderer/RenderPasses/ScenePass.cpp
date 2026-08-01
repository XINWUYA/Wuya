#include "Pch.h"
#include "ScenePass.h"
#include <Helios/Renderer/FrameGraph/FrameGraph.h>
#include <Helios/Renderer/RenderView.h>
#include <Helios/Renderer/Renderer.h>
#include <Helios/Scene/Mesh.h>
#include <Helios/Scene/Material.h>

namespace Helios
{
	namespace Forward
	{
		struct ScenePassData
		{
			FrameGraphResourceHandleTyped<FrameGraphTexture> ShadowMapHandle; /* 阴影贴图 */
		};

		void AddScenePass(RenderView* render_view, bool shadow_pass_enabled)
		{
			if (!render_view) return;

			auto& frame_graph = render_view->GetFrameGraph();
			if (!frame_graph) return;

			frame_graph->AddPass<ScenePassData>("ScenePass",
				[&, shadow_pass_enabled](FrameGraphBuilder& builder, ScenePassData& data)
				{
					if (shadow_pass_enabled)
					{
						data.ShadowMapHandle = frame_graph->GetBlackboard().GetResourceHandle<FrameGraphTexture>("ShadowMapHandle");
						builder.BindInputResource(data.ShadowMapHandle, FrameGraphTexture::Usage::Sampleable);
					}
					builder.AsSideEffect();
				},
				[&, render_view, shadow_pass_enabled](const FrameGraphResources& resources, const ScenePassData& data)
				{
					auto render_api = Renderer::GetRenderAPI();
					{
						render_api->Clear();
						auto& viewport_region = render_view->GetViewportRegion();
						render_api->SetViewport(0, 0, viewport_region.Width, viewport_region.Height);
						render_api->SetScissor(0, 0, viewport_region.Width, viewport_region.Height);

						for (const auto& mesh_object : render_view->GetVisibleMeshObjects())
						{
							Renderer::FillObjectUniformBuffer(mesh_object);
							auto& material = mesh_object.MeshSegment->GetMaterial();
							if (shadow_pass_enabled)
							{
								/* 指定阴影图 */
								material->SetParameters(ParamType::Texture, "u_ShadowMap", resources.Get(data.ShadowMapHandle).Texture);
							}
							Renderer::Submit(material, mesh_object.MeshSegment->GetMeshPrimitive());
						}
					}
				}
			);
		}

	}
}