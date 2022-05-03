#pragma once
#include "Wuya/Core/Window.h"

struct GLFWwindow;

namespace Wuya
{
	class IRenderContext;

	class GLWindow : public IWindow
	{
	public:
		GLWindow(const WindowConfig& config);
		virtual ~GLWindow();

		void OnUpdate() override;

		virtual uint32_t GetWidth() const override { return m_WindowInfo.Width; }
		virtual uint32_t GetHeight() const override { return m_WindowInfo.Height; }
		virtual bool IsVSync() const override;
		virtual void SetVSync(bool enable) override;

		virtual void* GetNativeWindow() const override { return m_pGLFWWindow; }
		virtual void SetEventCallback(const EventCallbackFunc& callback) override { m_WindowInfo.CallBackFunc = callback; }

	private:
		void Init(const WindowConfig& config);
		void ShutDown();

		struct WindowInfo
		{
			std::string Title;
			uint32_t Width, Height;
			bool IsVSync;
			EventCallbackFunc CallBackFunc;
		};
		GLFWwindow* m_pGLFWWindow{ nullptr };
		WindowInfo m_WindowInfo{};
		UniquePtr<IRenderContext> m_pRenderContext{ nullptr };
	};

}