#include "SampleSkyBox.h"
#include <imgui.h>

#include <Wuya/Scene/Material.h>
#include "SampleCamera.h"

namespace Wuya
{

void SampleSkyBox::OnAttached()
{
	Renderer::Init();

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

	SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
	vertex_buffer->SetLayout(vertex_buffer_layout);

	auto vertex_array = VertexArray::Create();
	vertex_array->AddVertexBuffer(vertex_buffer);

	TextureLoadConfig load_config{
		true,
		false,
		true,
		false,
		SamplerType::Sampler2D,
		SamplerWrapMode::ClampToEdge,
		SamplerMinFilter::Linear,
		SamplerMagFilter::Linear
	};
	auto sky_texture = Texture::Create(ABSOLUTE_PATH("Textures/drakensberg_solitary_mountain_4k.hdr"), load_config);

	auto shader = ShaderAssetManager::Instance().GetOrLoad(ABSOLUTE_PATH("Shaders/SkyBox.glsl"));
	auto material = Material::Create(shader);
	material->SetTexture("u_SkyTex", sky_texture);
	RenderRasterState raster_state;
	raster_state.EnableDepthWrite = true;
	raster_state.DepthCompareFunc = CompareFunc::LessEqual;
	raster_state.CullMode = CullMode::Cull_None;
	material->SetRasterState(raster_state);

	auto segment = MeshSegment::Create("SkyBoxMesh", vertex_array, material);

	auto skybox = Model::Create(BuiltinModelType::Sphere, material);
	// skybox->AddMeshSegment(segment);

	m_pScene = CreateSharedPtr<Scene>();
	auto entity = m_pScene->CreateEntity("SkyBox");
	auto& model_component = entity.AddComponent<ModelComponent>();
	model_component.Model = skybox;

	m_pCamera = CreateSharedPtr<SampleCamera>();
	auto& window = Application::Instance()->GetWindow();
	m_pCamera->SetViewportRegion({ 0,0, window.GetWidth(), window.GetHeight() });

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
	m_pScene->OnUpdateEditor(m_pCamera.get(), delta_time);
}

void SampleSkyBox::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample SkyBox: Show SkyBox.");
	ImGui::Text("- Alt + Mouse Left: Rotate.");
	ImGui::End();
}
}