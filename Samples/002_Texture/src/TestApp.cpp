#include "TestApp.h"
#include <imgui.h>

/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Helios/Application/EntryPoint.h>

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttached()
{
	// Texture
	m_pTexture2D = Helios::Texture2D::Create("assets/textures/game-sky.jpg");

	// Shader
	m_pShaderLibrary = Helios::CreateUniquePtr<Helios::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/texture.glsl");
	shader->Bind();
	shader->SetInt("u_Texture", m_pTexture2D->GetTextureID());

	Helios::Renderer::Init();

	// Vertex Array
	m_pVertexArray = Helios::VertexArray::Create();
	m_pVertexArray->Bind();

	// Triangle vertices
	const float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};
	Helios::SharedPtr<Helios::VertexBuffer> vertex_buffer = Helios::VertexBuffer::Create(vertices, sizeof(vertices));
	Helios::VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", Helios::BufferDataType::Float3 },
		{ "a_TexCoord", Helios::BufferDataType::Float2 }
	};
	vertex_buffer->SetLayout(vertex_buffer_layout);
	m_pVertexArray->AddVertexBuffer(vertex_buffer);

	// Indices
	const uint32_t indices[] = {
		0, 1, 2,
		1, 3, 2
	};
	Helios::SharedPtr<Helios::IndexBuffer> index_buffer = Helios::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_pVertexArray->SetIndexBuffer(index_buffer);
}

void TestLayer::OnDetached()
{
	ILayer::OnDetached();
}

void TestLayer::OnUpdate(float delta_time)
{
	Helios::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Helios::Renderer::Clear();

	m_pTexture2D->Bind(m_pTexture2D->GetTextureID());
	Helios::Renderer::Submit(m_pShaderLibrary->GetShaderByName("texture"), m_pVertexArray);
}

void TestLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample 002: Sample a texture.");
	ImGui::End();
}

TestApp::TestApp() : Application("002_Texture")
{
	PushLayer(Helios::CreateSharedPtr<TestLayer>());
}
