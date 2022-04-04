#include "TestApp.h"
#include <imgui.h>

/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttached()
{
	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/triangle.glsl");
	shader->Bind();

	Wuya::Renderer::Init();

	// Vertex Array
	m_pVertexArray = Wuya::VertexArray::Create();
	m_pVertexArray->Bind();

	// Triangle vertices
	const float vertices[3 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	Wuya::SharedPtr<Wuya::VertexBuffer> vertex_buffer = Wuya::VertexBuffer::Create(vertices, sizeof(vertices));
	Wuya::VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", Wuya::BufferDataType::Float3 }
	};
	vertex_buffer->SetLayout(vertex_buffer_layout);
	m_pVertexArray->AddVertexBuffer(vertex_buffer);

	// Indices
	const uint32_t indices[3] = {
		0, 1, 2
	};
	Wuya::SharedPtr<Wuya::IndexBuffer> index_buffer = Wuya::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_pVertexArray->SetIndexBuffer(index_buffer);
}

void TestLayer::OnDetached()
{
	ILayer::OnDetached();
}

void TestLayer::OnUpdate(float delta_time)
{
	Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();

	Wuya::Renderer::Submit(m_pShaderLibrary->GetShaderByName("triangle"), m_pVertexArray);
}

void TestLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample 001: Draw a triangle.");
	ImGui::End();
}

void TestLayer::OnEvent(Wuya::IEvent* event)
{
	ILayer::OnEvent(event);
}

TestApp::TestApp() : Application("001_Triangle")
{
	PushLayer(Wuya::CreateSharedPtr<TestLayer>());
}
