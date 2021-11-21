#include "pch.h"
#include "Application.h"
#include <Wuya/Events/ApplicationEvent.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Wuya 
{
	Application::Application(const std::string& window_title)
	{
		m_pWindow = IWindow::Create(WindowConfig(window_title));
		m_pWindow->SetEventCallback(BIND_EVENT_FUNC(Application::OnHandleEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_IsRuning)
		{
			float time = (float)glfwGetTime();
			float timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (auto* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_pWindow->OnUpdate();
		}
	}

	void Application::PushLayer(ILayer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(ILayer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnHandleEvent(IEvent* event)
	{
		//CORE_LOG_INFO("{0}", *event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowCloseEvent));

		// 从最上层Layer向最下层Layer传递
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event->Handled)
				break;
			
			(*it)->OnEvent(event);
		}

	}

	bool Application::OnHandleWindowCloseEvent(IEvent* event)
	{
		m_IsRuning = false;
		return true;
	}
}