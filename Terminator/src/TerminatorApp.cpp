#include "TerminatorApp.h"
/* 程序主入口点，需要保证放在Wuya.h之后 */
#include <Wuya/Application/EntryPoint.h>
#include <Wuya/Events/ApplicationEvent.h>

#include "EditorLayer.h"
#include <glad/glad.h>

#include "GLFW/glfw3.h"

TerminatorApp::TerminatorApp() : Application("Terminator")
{
	PushLayer(new EditorLayer());

	// Shader
	m_pShaderLibrary = Wuya::CreateUniquePtr<Wuya::ShaderLibrary>();
	auto shader = m_pShaderLibrary->Load("assets/shaders/triangle.glsl");
	shader->Bind();
	shader->SetFloat3("color", glm::vec3(1, 1, 0));

	// triangle vertices
	const float vertices[3 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f 
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// indices
	const unsigned int indices[3] = {
		0, 1, 2
	};

	glGenBuffers(1, &m_Index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void TerminatorApp::Run()
{
	while (m_IsRuning)
	{
		const float time = (float)glfwGetTime();
		const float timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
