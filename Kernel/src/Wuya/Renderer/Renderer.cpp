#include "Pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "FrameBuffer.h"
#include "Renderer2D.h"
#include "RenderView.h"
#include "Texture.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "FrameGraph/FrameGraph.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/Mesh.h"

namespace Wuya
{
	
	/* GlobalUniformBuffer的绑定位置 */
	namespace UniformBufferBindingPoint
	{
		constexpr uint8_t View			= 0;
		constexpr uint8_t Object		= 1;
		constexpr uint8_t Material		= 2;
		constexpr uint8_t Light			= 3;
	}

	/* Per view uniform data */
	struct ViewUniformData
	{
		glm::mat4 ViewProjectionMatrix{ 1 };
		uint32_t FrameCounter{ 0 }; /* 当前帧数计数 */
	};

	/* Per object uniform data */
	struct ObjectUniformData
	{
		glm::mat4 LocalToWorldMat{ 1 };	/* 模型矩阵 */
		uint32_t ObjectId{ 0 };				/* 模型ID，用于Picking */
	};

	/* Per light uniform data */
	struct LightUniformData
	{
		glm::vec4 ColorIntensity{ 1.0f, 1.0f, 1.0f, 1.0f }; /* rgb: color, a: intensity */
		glm::vec3 LightDir{ 0.0f };
		glm::vec3 LightPos{ 0.0f };
		uint32_t LightType{ 0 };
	};

	struct RenderData
	{
		/* 全局UniformBuffers */
		ViewUniformData ViewUniformData;
		SharedPtr<UniformBuffer> pViewUniformBuffer;
		SharedPtr<UniformBuffer> pObjectUniformBuffer;
		SharedPtr<UniformBuffer> pLightUniformBuffer;
	};

	static RenderData s_RenderData;

	SharedPtr<RenderAPI> Renderer::m_pRenderAPI = RenderAPI::Create();

	void Renderer::Init()
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->Init();
		Renderer2D::Init();

		s_RenderData.pViewUniformBuffer = UniformBuffer::Create(sizeof(ViewUniformData), UniformBufferBindingPoint::View);
		s_RenderData.pObjectUniformBuffer = UniformBuffer::Create(sizeof(ObjectUniformData), UniformBufferBindingPoint::Object);
		s_RenderData.pLightUniformBuffer = UniformBuffer::Create(sizeof(LightUniformData), UniformBufferBindingPoint::Light);
	}

	void Renderer::Update()
	{
		PROFILE_FUNCTION();

		s_RenderData.ViewUniformData.FrameCounter++;
	}

	void Renderer::Release()
	{
		PROFILE_FUNCTION();

		Renderer2D::Release();
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
	void Renderer::RenderAView(RenderView* view)
	{
		PROFILE_FUNCTION();

		/* Fill view uniform buffer */
		s_RenderData.ViewUniformData.ViewProjectionMatrix = view->GetCullingCamera()->GetViewProjectionMatrix();
		s_RenderData.pViewUniformBuffer->SetData(&s_RenderData.ViewUniformData, sizeof(ViewUniformData));

		view->Execute();
		//const auto viewport_region = view->GetViewportRegion();

		//FrameGraphTexture::Descriptor color_target_desc;
		//color_target_desc.Width = 1920;// viewport_region.Width();
		//color_target_desc.Height = 1080;// viewport_region.Height();
		//color_target_desc.TextureFormat = TextureFormat::RGBA8;

		//FrameGraphTexture::Descriptor depth_target_desc;
		//depth_target_desc.Width = 1920;// viewport_region.Width();
		//depth_target_desc.Height = 1080;// viewport_region.Height();
		//depth_target_desc.TextureFormat = TextureFormat::Depth32;

		///* 组装 FrameGraph */
		//FrameGraph frame_graph;

		///* Main Pass */
		//struct MainPassData
		//{
		//	//FrameGraphResourceHandleTyped<FrameGraphTexture> InputTexture;
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> GBufferAlbedo;
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> GBufferNormal;
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> GBufferRoughness;
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> GBufferDepth;
		//};

		//auto& main_pass = frame_graph.AddPass<MainPassData>("GBufferPass",
		//	[&](FrameGraphBuilder& builder, MainPassData& data)
		//	{
		//		//data.InputTexture = builder.CreateTexture("InputTexture", color_target_desc);
		//		data.GBufferAlbedo = builder.CreateTexture("GBufferAlbedo", color_target_desc);
		//		data.GBufferNormal = builder.CreateTexture("GBufferNormal", color_target_desc);
		//		data.GBufferRoughness = builder.CreateTexture("GBufferRoughness", color_target_desc);
		//		data.GBufferDepth = builder.CreateTexture("GBufferDepth", depth_target_desc);
		//		//builder.BindInputResource(data.InputTexture);
		//		builder.BindOutputResource(data.GBufferAlbedo, FrameGraphTexture::Usage::ColorAttachment);
		//		builder.BindOutputResource(data.GBufferNormal, FrameGraphTexture::Usage::ColorAttachment);
		//		builder.BindOutputResource(data.GBufferRoughness, FrameGraphTexture::Usage::ColorAttachment);
		//		builder.BindOutputResource(data.GBufferDepth, FrameGraphTexture::Usage::DepthAttachment);

		//		FrameGraphPassInfo::Descriptor pass_desc;
		//		pass_desc.Attachments.ColorAttachments[0] = data.GBufferAlbedo;
		//		pass_desc.Attachments.ColorAttachments[1] = data.GBufferNormal;
		//		pass_desc.Attachments.ColorAttachments[2] = data.GBufferRoughness;
		//		pass_desc.Attachments.DepthAttachment = data.GBufferDepth;
		//		pass_desc.ViewportRegion = viewport_region;
		//		builder.CreateRenderPass("GBufferPassRenderTarget", pass_desc);
		//		//builder.AsSideEffect();

		//		view->Prepare();
		//	},
		//	[&](const FrameGraphResources& resources, const MainPassData& data)
		//	{
		//		m_pRenderAPI->PushDebugGroup("GBufferPass");
		//		const auto& render_pass_info = resources.GetPassRenderTarget();
		//		render_pass_info->Bind();
		//		Clear();
		//		for (const auto& mesh_object : view->GetVisibleMeshObjects())
		//		{
		//			auto& material = mesh_object.MeshSegment->GetMaterial();
		//			auto& raster_state = material->GetRasterState();
		//			raster_state.CullMode = CullMode::Cull_Front;
		//			material->SetParameters("u_Local2WorldMat", mesh_object.Local2WorldMat);
		//			material->SetParameters("u_ViewProjectionMat", view->GetCullingCamera()->GetViewProjectionMatrix());
		//			//material->SetTexture(resources.Get(data.InputTexture).Texture, 1);
		//			Submit(material, mesh_object.MeshSegment->GetVertexArray());
		//		}
		//		render_pass_info->Unbind();
		//		m_pRenderAPI->Flush();
		//		m_pRenderAPI->PopDebugGroup();
		//	}
		//);
		//frame_graph.GetBlackboard()["GBufferAlbedo"] = main_pass.GetData().GBufferAlbedo;
		//frame_graph.GetBlackboard()["GBufferNormal"] = main_pass.GetData().GBufferNormal;
		//frame_graph.GetBlackboard()["GBufferRoughness"] = main_pass.GetData().GBufferRoughness;

		///* Side Pass */
		//struct SidePassData
		//{
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> InputTexture;
		//	FrameGraphResourceHandleTyped<FrameGraphTexture> OutputTexture;
		//};

		//auto& side_pass = frame_graph.AddPass<SidePassData>("SidePass",
		//	[&](FrameGraphBuilder& builder, SidePassData& data)
		//	{
		//		data.InputTexture = frame_graph.GetBlackboard().GetResourceHandle<FrameGraphTexture>("GBufferAlbedo");
		//		data.OutputTexture = builder.CreateTexture("OutputTexture", color_target_desc);

		//		builder.BindInputResource(data.InputTexture, FrameGraphTexture::Usage::Sampleable);
		//		builder.BindOutputResource(data.OutputTexture, FrameGraphTexture::Usage::ColorAttachment);
		//		builder.AsSideEffect();

		//		FrameGraphPassInfo::Descriptor pass_desc;
		//		pass_desc.Attachments.ColorAttachments[0] = data.OutputTexture;
		//		pass_desc.ViewportRegion = viewport_region;
		//		builder.CreateRenderPass("SidePassRenderTarget", pass_desc);
		//	},
		//	[&](const FrameGraphResources& resources, const SidePassData& data)
		//	{
		//		m_pRenderAPI->PushDebugGroup("SidePass");
		//		Clear();
		//		SharedPtr<Material> material = CreateSharedPtr<Material>();
		//		auto& raster_state = material->GetRasterState();
		//		raster_state.EnableDepthWrite = false;
		//		static const auto shader = Shader::Create("assets/shaders/side.glsl");
		//		material->SetShader(shader);
		//		material->SetTexture(resources.Get(data.InputTexture).Texture, 0);
		//		material->Bind();
		//		Submit(material, GetFullScreenVertexArray());
		//		m_pRenderAPI->PopDebugGroup();
		//	}
		//);

		///* 执行FrameGraph */
		//frame_graph.Build();
		//frame_graph.Execute();

		//frame_graph.ExportGraphviz("framegraph.txt");

		///* 输出 */
		//auto output_handle = side_pass.GetData().OutputTexture;
		//auto output_rt = static_cast<const Resource<FrameGraphTexture>*>(frame_graph.GetResource(output_handle))->GetResource().Texture;
		//view->SetRenderTargetTexture(output_rt);

	}

	void Renderer::Submit(const SharedPtr<Material>& material, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count)
	{
		PROFILE_FUNCTION();

		material->Bind();
		vertex_array->Bind();

		m_pRenderAPI->ApplyRasterState(material->GetRasterState());

		if (vertex_array->GetIndexBuffer())
			m_pRenderAPI->DrawIndexed(vertex_array, index_count);
		else
			m_pRenderAPI->DrawArrays(vertex_array);

		material->Unbind();
		vertex_array->Unbind();
	}

	SharedPtr<VertexArray> Renderer::GetFullScreenVertexArray()
	{
		PROFILE_FUNCTION();

		/* Vertices */
		static constexpr float vertices[3 * 4] = 
		{
			-1.0f, -1.0f, 1.0f, 1.0f,
			 3.0f, -1.0f, 1.0f, 1.0f,
			-1.0f,  3.0f, 1.0f, 1.0f,
		};
		static const VertexBufferLayout vertex_buffer_layout = 
		{
			{ "a_Position", BufferDataType::Float4 }
		};
		const auto vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
		vertex_buffer->SetLayout(vertex_buffer_layout);

		/* Indices */
		static constexpr uint32_t indices[3] = 
		{
			0, 1, 2
		};
		const auto index_buffer = IndexBuffer::Create(indices, 3);

		auto vertex_array = VertexArray::Create();
		vertex_array->AddVertexBuffer(vertex_buffer);
		vertex_array->SetIndexBuffer(index_buffer);
		return vertex_array;
	}

	void Renderer::FillObjectUniformBuffer(const VisibleMeshObject& mesh_object)
	{
		static ObjectUniformData data;
		data.LocalToWorldMat = mesh_object.Local2WorldMat;
		data.ObjectId = mesh_object.ObjectId;
		s_RenderData.pObjectUniformBuffer->SetData(&data, sizeof(ObjectUniformData));
	}

	void Renderer::FillLightUniformBuffer(const ValidLight& valid_light)
	{
		static LightUniformData data;
		data.ColorIntensity = valid_light.ColorIntensity;
		data.LightDir = valid_light.LightDir;
		data.LightPos = valid_light.LightPos;
		data.LightType = valid_light.LightType;
		s_RenderData.pLightUniformBuffer->SetData(&data, sizeof(LightUniformData));
	}
}
