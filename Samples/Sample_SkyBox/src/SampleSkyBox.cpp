#include "SampleSkyBox.h"
#include <imgui.h>

#include <Helios/Scene/Material.h>
#include "SampleCamera.h"

namespace Helios
{

void SampleSkyBox::OnAttached()
{
	// Cube vertices
	const float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", BufferDataType::Float3 },
		{ "a_TexCoord", BufferDataType::Float2 }
	};

	SharedPtr<DeviceVertexBuffer> vertex_buffer = DeviceVertexBuffer::Create(vertices, sizeof(vertices));
	vertex_buffer->SetLayout(vertex_buffer_layout);

	auto vertex_array = DeviceVertexArray::Create();
	vertex_array->AddVertexBuffer(vertex_buffer);

	TextureLoadConfig load_config{
		.IsFlipV=true,
		.IsGenMips=false,
		.IsHdr=true,
		.IsSrgb=false,
		.SamplerType=SamplerType::Sampler2D,
		.SamplerWrapMode=SamplerWrapMode::ClampToEdge,
		.SamplerMinFilter=SamplerMinFilter::Linear,
		.SamplerMagFilter=SamplerMagFilter::Linear
	};
	auto sky_texture = DeviceTexture::Create(ABSOLUTE_PATH("Textures/drakensberg_solitary_mountain_4k.hdr"), load_config);

	auto shader = ShaderAssetManager::Instance().GetOrLoad(ABSOLUTE_PATH("Shaders/SkyBox.glsl"));
	auto material = Material::Create(shader);
	material->SetTexture("u_SkyTex", sky_texture, 0);
	RenderRasterState raster_state;
	raster_state.EnableDepthWrite = true;
	raster_state.DepthCompareFunc = CompareFunc::LessEqual;
	raster_state.CullMode = CullMode::Cull_None;
	material->SetRasterState(raster_state);

	auto segment = MeshSegment::Create("SkyBoxMesh", vertex_array, material);

	auto skybox = Model::Create(BuiltinModelType::Sphere, material);
	// skybox->AddMeshSegment(segment);

	m_pScene = CreateSharedPtr<Scene>();
	auto model_entity = m_pScene->CreateEntity("SkyBox");
	auto& model_component = model_entity.AddComponent<ModelComponent>();
	model_component.m_Model = skybox;

	m_pCamera = CreateSharedPtr<SampleCamera>();
	auto& window = Application::Instance()->GetWindow();

	auto camera_entity = m_pScene->CreateEntity("MainCamera");
	auto& camera_component = camera_entity.AddComponent<CameraComponent>();
	m_pCamera = camera_component.m_Camera;
	auto render_view = camera_component.m_Camera->GetRenderView();
	render_view->SetViewportRegion({ 0,0, window.GetWidth(), window.GetHeight() });


	auto& frame_graph = render_view->GetFrameGraph();
	frame_graph->Reset();

	/* Scene Pass */
	struct ScenePassData
	{
	};

	auto scene_pass = frame_graph->AddPass<ScenePassData>("ScenePass",
		[&](FrameGraphBuilder& builder, ScenePassData& data)
		{
			builder.AsSideEffect();
		},
		[&](const FrameGraphResources& resources, const ScenePassData& data)
		{
			auto render_api = Renderer::GetRenderAPI();
			render_api->PushDebugGroup("ScenePass");

			{
				auto render_view = m_pCamera->GetRenderView();
				auto& viewport_region = render_view->GetViewportRegion();
				render_api->Clear();
				render_api->SetViewport(0, 0, viewport_region.Width, viewport_region.Height);
				render_api->SetScissor(0, 0, viewport_region.Width, viewport_region.Height);

				for (const auto& mesh_object : render_view->GetVisibleMeshObjects())
				{
					/* Fill object uniform buffer */
					Renderer::FillObjectUniformBuffer(mesh_object);

					auto& material = mesh_object.MeshSegment->GetMaterial();
					Renderer::Submit(material, mesh_object.MeshSegment->GetMeshPrimitive());
				}
			}

			render_api->PopDebugGroup();
		});

	/* ImGui Pass */
	if (const auto& imgui_layer = Application::Instance()->GetImGuiLayer())
	{
		imgui_layer->AddFrameGraphPass(*frame_graph);
	}

	// frame_graph->ExportGraphviz("framegraph.txt");
	render_view->Prepare();
}

void SampleSkyBox::OnDetached()
{
	ILayer::OnDetached();
}

void SampleSkyBox::OnUpdate(float delta_time)
{
	Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Renderer::Clear();

	m_pCamera->OnUpdate(delta_time);
	m_pScene->OnUpdateRuntime(delta_time);
	m_pScene->Render();
}

void SampleSkyBox::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample SkyBox: Show SkyBox.");
	ImGui::Text("- Alt + Mouse Left: Rotate.");
	ImGui::End();

	/* 若当前ImGui窗口不是主窗口，应阻塞事件传递 */
	Application::Instance()->GetImGuiLayer()->BlockEvents(!ImGui::IsWindowFocused() && !ImGui::IsWindowHovered());
}
}