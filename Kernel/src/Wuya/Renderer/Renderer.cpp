#include "Pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Renderer2D.h"
#include "RenderView.h"
#include "Texture.h"
#include "FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/Mesh.h"

namespace Wuya
{
	UniquePtr<RenderAPI> Renderer::m_pRenderAPI = RenderAPI::Create();

	void Renderer::Init()
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->Init();
		Renderer2D::Init();
	}

	void Renderer::Update()
	{
		PROFILE_FUNCTION();
	}

	void Renderer::SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->SetViewport(x_start, y_start, width, height);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->SetClearColor(color);
	}

	void Renderer::Clear()
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->Clear();
	}

	/* 绘制一个视图 */
	void Renderer::RenderAView(const SharedPtr<RenderView>& view)
	{
		const auto viewport_region = view->GetViewportRegion();

		FrameGraphTexture::Descriptor color_target_desc;
		color_target_desc.Width = 1920;// viewport_region.Width();
		color_target_desc.Height = 1080;// viewport_region.Height();
		color_target_desc.TextureFormat = TextureFormat::R10G10B10A2;

		FrameGraphTexture::Descriptor depth_target_desc;
		depth_target_desc.Width = 1920;// viewport_region.Width();
		depth_target_desc.Height = 1080;// viewport_region.Height();
		depth_target_desc.TextureFormat = TextureFormat::Depth32;

		/* 组装 FrameGraph */
		FrameGraph frame_graph;

		/* Main Pass */
		struct MainPassData
		{
			FrameGraphResourceHandleTyped<FrameGraphTexture> InputTexture;
			FrameGraphResourceHandleTyped<FrameGraphTexture> ColorTexture;
			FrameGraphResourceHandleTyped<FrameGraphTexture> DepthTexture;
		};

		auto& main_pass = frame_graph.AddPass<MainPassData>("MainPass",
			[&](FrameGraphBuilder& builder, MainPassData& data)
			{
				data.InputTexture = builder.CreateTexture("InputTexture");
				data.ColorTexture = builder.CreateTexture("ColorTexture", color_target_desc);
				data.DepthTexture = builder.CreateTexture("DepthTexture", depth_target_desc);
				builder.BindInputResource(data.InputTexture);
				builder.BindOutputResource(data.ColorTexture, FrameGraphTexture::Usage::ColorAttachment);
				builder.BindOutputResource(data.DepthTexture, FrameGraphTexture::Usage::DepthAttachment);
				builder.AsSideEffect();

				FrameGraphPassInfo::Descriptor pass_desc;
				pass_desc.Attachments.ColorAttachments[0] = data.ColorTexture;
				pass_desc.Attachments.DepthAttachment = data.DepthTexture;
				pass_desc.ViewportRegion = viewport_region;
				builder.CreateRenderPass("MainPassRenderTarget", pass_desc);
				view->Prepare();
			},
			[&](const FrameGraphResources& resources, const MainPassData& data)
			{
				for (const auto& mesh_object : view->GetVisibleMeshObjects())
				{
					auto& material = mesh_object.MeshSegment->GetMaterial();
					material->SetParameters("u_Local2WorldMat", mesh_object.Local2WorldMat);
					material->SetParameters("u_ViewProjectionMat", view->GetCullingCamera()->GetViewProjectionMatrix());
					material->SetTexture(resources.Get(data.InputTexture).Texture, 1);
					material->Bind();
					Submit(material->GetShader(), mesh_object.MeshSegment->GetVertexArray());
				}
			}
		);

		/* 输出 */
		auto ouput = main_pass.GetData().ColorTexture;

		/* 执行FrameGraph */
		frame_graph.Build();
		frame_graph.Execute();

		frame_graph.ExportGraphviz("framegraph.txt");
	}

	void Renderer::Submit(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count)
	{
		PROFILE_FUNCTION();

		shader->Bind();
		vertex_array->Bind();

		if (vertex_array->GetIndexBuffer())
			m_pRenderAPI->DrawIndexed(vertex_array, index_count);
		else
			m_pRenderAPI->DrawArrays(vertex_array);
	}
}
