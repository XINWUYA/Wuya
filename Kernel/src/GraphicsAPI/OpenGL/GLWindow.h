#pragma once
#include "Wuya/Core/Window.h"
#include "Wuya/Renderer/RenderContext.h"

struct GLFWwindow;

namespace Wuya
{
	/* GLWindow类：
	 * 使用GLFWwindow创建OpenGL窗口
	 */
	class GLWindow : public IWindow
	{
	public:
		GLWindow(const WindowDesc& desc);
		~GLWindow() override;

		/* 更新，交换一帧 */
		void OnUpdate() override;

		/* 获取宽度/高度 */
		[[nodiscard]] uint32_t GetWidth() const override { return m_WindowInfo.Descriptor.Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_WindowInfo.Descriptor.Height; }

		/* 设置垂直同步 */
		[[nodiscard]] bool IsVSync() const override { return m_WindowInfo.Descriptor.IsVSync; }
		void SetVSync(bool enable) override;

		/* 获取GLFWwindow* */
		[[nodiscard]] void* GetNativeWindow() const override { return m_pGLFWWindow; }

		/* 设置响应事件 */
		virtual void SetEventCallback(const EventCallbackFunc& callback) override { m_WindowInfo.CallBackFunc = callback; }

	private:
		/* 创建窗口；创建上下文；绑定响应事件 */
		void Build(const WindowDesc& desc);
		/* 销毁窗口 */
		void Destroy();

		/* 窗口信息 */
		struct WindowInfo
		{
			WindowDesc Descriptor;			/* 窗口描述 */
			EventCallbackFunc CallBackFunc; /* 窗口事件回调 */
		};

		/* GLFWwindow指针 */
		GLFWwindow* m_pGLFWWindow{ nullptr };
		/* 窗口信息*/
		WindowInfo m_WindowInfo{};
		/* 窗口上下文 */
		UniquePtr<IRenderContext> m_pRenderContext{ nullptr };
	};

}