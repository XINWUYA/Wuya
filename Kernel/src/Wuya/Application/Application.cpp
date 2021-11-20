#include "pch.h"
#include "Application.h"
#include "Wuya/Events/ApplicationEvent.h"

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
			m_pWindow->OnUpdate();

		}
	}

	void Application::OnHandleEvent(IEvent* event)
	{
		//CORE_LOG_INFO("{0}", *event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowCloseEvent));

	}

	bool Application::OnHandleWindowCloseEvent(IEvent* event)
	{
		m_IsRuning = false;
		return true;
	}
}