#include "EditorLayer.h"
#include <imgui.h>

EditorLayer::EditorLayer()
	: ILayer("EditorLayer")
{
}

void EditorLayer::OnAttached()
{
	// Texture
	m_pTexture2D = Wuya::Texture2D::Create("assets/textures/game-sky.jpg");

	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/triangle.glsl");
	shader->Bind();
	shader->SetFloat3("color", glm::vec3(1, 1, 0));
	shader->SetInt("u_Texture", 0);

	Wuya::Renderer::Init();

	m_pVertexArray = Wuya::VertexArray::Create();
	m_pVertexArray->Bind();

	// triangle vertices
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

	// Camera
	m_pEditorCamera = Wuya::CreateUniquePtr<Wuya::EditorCamera>(30.0f);
	//m_pEditorCamera->SetViewportSize(1280, 720);
	//m_pEditorCamera->SetDistance(1);

	// Frame buffer
	Wuya::FrameBufferDescription desc;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Attachments = { Wuya::FrameBufferTargetFormat::RGBA8, Wuya::FrameBufferTargetFormat::RedInteger, Wuya::FrameBufferTargetFormat::Depth24Stencil8 };
	m_pFrameBuffer = Wuya::FrameBuffer::Create(desc);
}

void EditorLayer::OnDetached()
{
	ILayer::OnDetached();
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	auto viewport_size = { viewport_panel_size.x, viewport_panel_size.y };

	uint64_t texture_id = m_pFrameBuffer->GetColorAttachmentByIndex(0);
	ImGui::Image(reinterpret_cast<void*>(texture_id), viewport_panel_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
}

void EditorLayer::OnEvent(Wuya::IEvent* event)
{
	m_pEditorCamera->OnEvent(event);
}

void EditorLayer::OnUpdate(float delta_time)
{
	m_pEditorCamera->OnUpdate();

	m_pFrameBuffer->Bind();

	Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();
	m_pFrameBuffer->ClearColorAttachment(1, -1);

	m_pTexture2D->Bind();

	auto shader = m_pShaderLibrary->GetShaderByName("triangle");
	/*glm::mat4 scale = glm::mat4(
		2, 0, 0, 0,
		0, 2, 0, 0,
		0, 0, 2, 0,
		0, 0, 0, 1);
	shader->SetMat4("u_ViewProjectionMat", scale);*/
	shader->SetMat4("u_ViewProjectionMat", m_pEditorCamera->GetViewProjection());
	Wuya::Renderer::Submit(shader, m_pVertexArray);

	m_pFrameBuffer->Unbind();
	/*Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();*/
}
