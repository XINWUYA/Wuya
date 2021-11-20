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

		virtual void OnHandleEvent(IEvent* event);
		virtual bool OnHandleWindowCloseEvent(IEvent* event);

	private:
		UniquePtr<IWindow> m_pWindow{ nullptr };
		bool m_IsRuning{ true };
	};

	// Implement in clients
	Application* CreateApplication();
}