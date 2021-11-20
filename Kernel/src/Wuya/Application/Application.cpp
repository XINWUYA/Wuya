#include "pch.h"
#include "Application.h"

namespace Wuya 
{
	Application::Application(const std::string& window_title)
	{
		m_Window = IWindow::Create(WindowConfig(window_title));
	}

	Application::~Application()
	{
	}
	void Application::Run()
	{
		while (true)
		{
			m_Window->OnUpdate();

		}
	}
}