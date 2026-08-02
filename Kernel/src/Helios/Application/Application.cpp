#include "Pch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Helios/Events/ApplicationEvent.h"
#include "Helios/ImGui/ImGuiLayer.h"
#include "Helios/Renderer/Renderer.h"

#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalRenderAPI.h"
#endif

namespace Helios 
{
	Application* Application::s_pInstance = nullptr;

	Application::Application(const std::string& window_title, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		ASSERT(!s_pInstance, "Application already exist!");
		s_pInstance = this;

		m_pWindow = DeviceWindow::Create({ window_title, width, height });
		m_pWindow->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));
		Renderer::Init();

		m_pImGuiLayer = CreateSharedPtr<ImGuiLayer>();
		PushOverlay(m_pImGuiLayer);
	}

	Application::~Application()
	{
		PROFILE_FUNCTION();

		m_LayerStack.Release();
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
#ifdef PLATFORM_MACOS
				/* Metal: 在帧开始时准备下一帧的drawable */
				auto metalRenderAPI = std::dynamic_pointer_cast<MetalRenderAPI>(Renderer::GetRenderAPI());
				if (metalRenderAPI)
				{
					metalRenderAPI->PrepareNextDrawable();
				}
#endif
				/* 先更新逻辑层和渲染层 */
				{
					PROFILE_SCOPE("Update Layers");

					for (const auto& layer : m_LayerStack)
						layer->OnUpdate(delta_time);
				}

				/* 后更新UI */
				m_pImGuiLayer->Begin();
				{
					PROFILE_SCOPE("Update ImGui Layers");

					for (const auto& layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_pImGuiLayer->End();
			}

			// SwapBuffer，显示帧画面到屏幕
			m_pWindow->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
	}

	void Application::PushLayer(const SharedPtr<ILayer>& layer)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(const SharedPtr<ILayer>& layer)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(IEvent* event)
	{
		PROFILE_FUNCTION();

		//CORE_LOG_INFO("{0}", *event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnHandleWindowResizeEvent));

		// 从上层Layer向下层Layer传递
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event->Handled) // 事件停止传递
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

		// 事件继续向下层传递，应返回false
		return false;
	}
}
