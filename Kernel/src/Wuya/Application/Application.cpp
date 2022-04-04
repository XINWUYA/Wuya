#include "pch.h"
#include "Application.h"
#include <Wuya/Events/ApplicationEvent.h>
#include <Wuya/Imgui/ImGuiLayer.h>
#include "Wuya/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Wuya 
{
	Application* Application::s_pInstance = nullptr;

	Application::Application(const std::string& window_title)
	{
		PROFILE_FUNCTION();

		CORE_ASSERT(!s_pInstance, "Application already exist!");
		s_pInstance = this;

		m_pWindow = IWindow::Create(WindowConfig(window_title));
		m_pWindow->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_pImGuiLayer = CreateSharedPtr<ImGuiLayer>();
		PushOverlay(m_pImGuiLayer);
	}

	Application::~Application()
	{
	}

	Application* Application::Instance()
	{
		return s_pInstance;
	}

	void Application::Run()
	{
		PROFILE_FUNCTION();

		while (m_IsRunning)
		{
			const float time = (float)glfwGetTime();
			const float timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_IsMinimized)
			{
				// Update layers
				{
					PROFILE_SCOPE("Update Layers");

					for (auto layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				// Update layers' gui 
				m_pImGuiLayer->Begin();
				{
					PROFILE_SCOPE("Update ImGui Layers");

					for (auto layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_pImGuiLayer->End();
			}

			// Update window
			m_pWindow->OnUpdate();
		}
	}

	void Application::PushLayer(SharedPtr<ILayer> layer)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttached();
	}

	void Application::PushOverlay(SharedPtr<ILayer> layer)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttached();
	}

	void Application::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		//CORE_LOG_INFO("{0}", *event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowResizeEvent));

		// 从最上层Layer向最下层Layer传递
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event->Handled) // 事件停止传递
				break;
			
			(*it)->OnEvent(event);
		}
	}

	bool Application::OnHandleWindowCloseEvent(IEvent* event)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnHandleWindowResizeEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		const auto resize_event = dynamic_cast<WindowResizeEvent*>(event);
		if (resize_event->GetWidth() == 0 || resize_event->GetHeight() == 0)
		{
			m_IsMinimized = true;
			return false;
		}

		m_IsMinimized = false;
		Renderer::SetViewport(0, 0, resize_event->GetWidth(), resize_event->GetHeight());

		// 事件仍需向下层传递，应返回false
		return false;
	}
}
