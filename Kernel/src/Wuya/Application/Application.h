#pragma once
#include <Wuya/Common/Utils.h>
#include <Wuya/Core/Window.h>
#include <Wuya/Core/LayerStack.h>

namespace Wuya
{
	class WUYA_API Application 
	{
	public:
		Application(const std::string& window_title = "Unnamed App");
		virtual ~Application();

		static Application* Instance();
		IWindow& GetWindow() { return *m_pWindow; }

		virtual void Run();

		virtual void PushLayer(ILayer* layer);
		virtual void PushOverlay(ILayer* layer);

		virtual void OnHandleEvent(IEvent* event);
		virtual bool OnHandleWindowCloseEvent(IEvent* event);

	private:
		static Application* s_Instance;
		UniquePtr<IWindow> m_pWindow{ nullptr };
		LayerStack m_LayerStack{};
		bool m_IsRuning{ true };
		float m_LastFrameTime{ 0.0f };
	};

	// Implement in clients
	Application* CreateApplication();
}