#include "Pch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Wuya/Events/ApplicationEvent.h"
#include "Wuya/ImGui/ImGuiLayer.h"
#include "Wuya/Renderer/Renderer.h"

namespace Wuya 
{
	Application* Application::s_pInstance = nullptr;

	Application::Application(const std::string& window_title)
	{
		PROFILE_FUNCTION();

		ASSERT(!s_pInstance, "Application already exist!");
		s_pInstance = this;

		WindowConfig config{ window_title, 1920, 1080 }; /* todo: �������ļ������л� */
		m_pWindow = IWindow::Create(config);
		m_pWindow->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_pImGuiLayer = CreateSharedPtr<ImGuiLayer>();
		PushOverlay(m_pImGuiLayer);
	}

	Application::~Application()
	{
		PROFILE_FUNCTION();

		Renderer::Release();
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
			PROFILE_SCOPE("Running Loops");

			const float time = static_cast<float>(glfwGetTime());
			const float delta_time = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_IsMinimized)
			{
				/* �ȸ����߼������Ⱦ�� */
				{
					PROFILE_SCOPE("Update Layers");

					for (const auto& layer : m_LayerStack)
						layer->OnUpdate(delta_time);
				}

				/* �����UI */
				m_pImGuiLayer->Begin();
				{
					PROFILE_SCOPE("Update ImGui Layers");

					for (const auto& layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_pImGuiLayer->End();
			}

			// ��Ⱦһ֡
			m_pWindow->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
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

		// �����ϲ�Layer�����²�Layer����
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event->Handled) // �¼�ֹͣ����
				break;
			
			(*it)->OnEvent(event);
		}
	}

	bool Application::OnHandleWindowCloseEvent(IEvent* event)
	{
		Close();
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

		// �¼��������²㴫�ݣ�Ӧ����false
		return false;
	}
}
