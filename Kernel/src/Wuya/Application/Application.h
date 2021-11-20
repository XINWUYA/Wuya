#pragma once
#include "Wuya/Common/Utils.h"
#include "Wuya/Core/Window.h"

namespace Wuya
{
	class WUYA_API Application 
	{
	public:
		Application(const std::string& window_title);
		virtual ~Application();

		virtual void Run();

	private:
		UniquePtr<IWindow> m_Window{ nullptr };
	};

	// Implement in clients
	Application* CreateApplication();
}