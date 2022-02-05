#include "TerminatorApp.h"
/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>
#include <Wuya/Events/ApplicationEvent.h>

#include "EditorLayer.h"
#include "GLFW/glfw3.h"

TerminatorApp::TerminatorApp() : Application("Terminator")
{
	PushLayer(new EditorLayer());

	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/triangle.glsl");
	shader->Bind();
	shader->SetFloat3("color", glm::vec3(1, 1, 0));

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

	// indices
	const uint32_t indices[3] = {
		0, 1, 2
	};
	Wuya::SharedPtr<Wuya::IndexBuffer> index_buffer = Wuya::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_pVertexArray->SetIndexBuffer(index_buffer);
}

void TerminatorApp::Run()
{
	while (m_IsRuning)
	{
		const float time = (float)glfwGetTime();
		const float timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
		Wuya::Renderer::Clear();

		Wuya::Renderer::Submit(m_pShaderLibrary->GetShaderByName("triangle"), m_pVertexArray);

		// Update layers
		for (auto* layer : m_LayerStack)
			layer->OnUpdate(timestep);

		// Update layers' gui 
		m_pImguiLayer->Begin();
		for (auto* layer : m_LayerStack)
			layer->OnImGuiRender();
		m_pImguiLayer->End();

		// Update window
		m_pWindow->OnUpdate();
	}

	CLIENT_LOG_WARN("Hello! {0}", 5);

	Wuya::WindowResizeEvent e(1280, 720);

	if (e.IsInCategory(Wuya::EventCategoryApplication))
	{
		CLIENT_LOG_DEBUG(e.GetName());
		CLIENT_LOG_CRITICAL(e);
	}
}
