#pragma once
#include "Wuya/Core/Window.h"
#include "Wuya/Core/LayerStack.h"

namespace Wuya
{
	class ImGuiLayer;

	class Application 
	{
	public:
		Application(const std::string& window_title = "Unnamed App", uint32_t width = 1920, uint32_t height = 1080);
		virtual ~Application();

		static Application* Instance();
		IWindow& GetWindow() { return *m_pWindow; }

		virtual void Run();
		void Close();

		void PushLayer(const SharedPtr<ILayer>& layer);
		void PushOverlay(const SharedPtr<ILayer>& layer);

		const SharedPtr<ImGuiLayer>& GetImGuiLayer() const { return m_pImGuiLayer; }

		virtual void OnEvent(IEvent* event);
		virtual bool OnHandleWindowCloseEvent(IEvent* event);
		virtual bool OnHandleWindowResizeEvent(IEvent* event);

	protected:
		static Application* s_pInstance;
		UniquePtr<IWindow> m_pWindow{ nullptr };
		SharedPtr<ImGuiLayer> m_pImGuiLayer{ nullptr };
		LayerStack m_LayerStack{};
		bool m_IsRunning{ true };
		bool m_IsMinimized{ false };
		float m_LastFrameTime{ 0.0f };
	};

	// Implement in clients
	UniquePtr<Application> CreateApplication();
}