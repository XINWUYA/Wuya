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
	m_pTexture2D = Helios::Texture2D::Create("assets/textures/container.jpg");

	// Shader
	m_pShaderLibrary = Helios::CreateUniquePtr<Helios::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/texture.glsl");
	shader->Bind();
	shader->SetInt("u_Texture", m_pTexture2D->GetTextureID());

	Helios::Renderer::Init();

	// Vertex Array
	m_pVertexArray = Helios::DeviceVertexArray::Create();
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
	Helios::SharedPtr<Helios::DeviceVertexBuffer> vertex_buffer = Helios::DeviceVertexBuffer::Create(vertices, sizeof(vertices));
	Helios::VertexBufferLayout vertex_buffer_layout = {
		{ "a_Position", Helios::BufferDataType::Float3 },
		{ "a_TexCoord", Helios::BufferDataType::Float2 }
	};
	vertex_buffer->SetLayout(vertex_buffer_layout);
	m_pVertexArray->AddVertexBuffer(vertex_buffer);

	// Camera
	m_pEditorCamera = Helios::CreateUniquePtr<Helios::EditorCamera>(30.0f);
	m_pEditorCamera->SetDistance(5.0f);

	// Uniform Buffer
	m_pCameraCBuffer = Helios::DeviceUniformBuffer::Create(sizeof(CameraParams), 0);

	// Frame buffer
	Helios::FrameBufferDescription desc;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Attachments = { Helios::FrameBufferTargetFormat::RGBA8, Helios::FrameBufferTargetFormat::RedInteger, Helios::FrameBufferTargetFormat::Depth24Stencil8 };
	m_pFrameBuffer = Helios::DeviceFrameBuffer::Create(desc);
}

void TestLayer::OnDetached()
{
	ILayer::OnDetached();
}

void TestLayer::OnUpdate(float delta_time)
{
	m_pEditorCamera->OnUpdate();

	m_pFrameBuffer->Bind();
	m_pFrameBuffer->ClearColorAttachment(1, -1);

	Helios::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Helios::Renderer::Clear();

	m_pTexture2D->Bind(m_pTexture2D->GetTextureID());

	m_CameraParams.ViewProjection = m_pEditorCamera->GetViewProjectionMatrix();
	m_pCameraCBuffer->SetData(&m_CameraParams, sizeof(CameraParams));

	Helios::Renderer::Submit(m_pShaderLibrary->GetShaderByName("texture"), m_pVertexArray);

	m_pFrameBuffer->Unbind();
	Helios::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Helios::Renderer::Clear();
}

void TestLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::Text("Sample 004: Frame buffer.");

	const ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	const uint64_t texture_id = m_pFrameBuffer->GetColorAttachmentByIndex(0);
	ImGui::Image(reinterpret_cast<void*>(texture_id), viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
}

TestApp::TestApp() : Application("004_FrameBuffer")
{
	PushLayer(Helios::CreateSharedPtr<TestLayer>());
}
