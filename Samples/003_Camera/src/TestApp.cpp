#include "TestApp.h"
#include <imgui.h>

#include "Wuya/Scene/Material.h"

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttached()
{
	// Texture
	m_pTexture2D = Wuya::Texture2D::Create("assets/textures/container.jpg");

	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/texture.glsl");
	shader->Bind();
	shader->SetInt("u_Texture", m_pTexture2D->GetTextureID());

	Wuya::Renderer::Init();

	// Vertex Array
	m_pVertexArray = Wuya::VertexArray::Create();
	m_pVertexArray->Bind();

	// Triangle vertices
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
	Wuya::SharedPtr<Wuya::VertexBuffer> vertex_buffer = Wuya::VertexBuffer::Create(vertices, sizeof(vertices));
	Wuya::VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", Wuya::BufferDataType::Float3 },
		{ "a_TexCoord", Wuya::BufferDataType::Float2 }
	};
	vertex_buffer->SetLayout(vertex_buffer_layout);
	m_pVertexArray->AddVertexBuffer(vertex_buffer);

	// Camera
	m_pEditorCamera = Wuya::CreateUniquePtr<Wuya::EditorCamera>(30.0f);
	m_pEditorCamera->SetDistance(5.0f);

	// Uniform Buffer
	m_pCameraCBuffer = Wuya::UniformBuffer::Create(sizeof(CameraParams), 0);
	
}

void TestLayer::OnDetached()
{
	ILayer::OnDetached();
}

void TestLayer::OnUpdate(float delta_time)
{
	m_pEditorCamera->OnUpdate(delta_time);

	Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();

	m_pTexture2D->Bind(m_pTexture2D->GetTextureID());

	m_CameraParams.ViewProjection = m_pEditorCamera->GetViewProjectionMatrix();
	m_pCameraCBuffer->SetData(&m_CameraParams, sizeof(CameraParams));

	Wuya::SharedPtr<Wuya::Material> material = Wuya::CreateSharedPtr<Wuya::Material>();
	auto shader = m_pShaderLibrary->GetShaderByName("texture");
	material->SetShader(shader);
	Wuya::Renderer::Submit(material, m_pVertexArray);
}

void TestLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample 003: Camera control.");
	ImGui::Text("- Press Mouse Left: Rotate.");
	ImGui::Text("- Press Mouse Middle: Pan.");
	ImGui::Text("- Press Mouse Right: Zoom.");
	ImGui::End();
}

TestApp::TestApp() : Application("003_Camera")
{
	PushLayer(Wuya::CreateSharedPtr<TestLayer>());
}
