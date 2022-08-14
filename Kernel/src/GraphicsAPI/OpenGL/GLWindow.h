#pragma once
#include "Wuya/Core/Window.h"

struct GLFWwindow;

namespace Wuya
{
	class IRenderContext;

	/* GLWindow�ࣺ
	 * ʹ��GLFWwindow����OpenGL����
	 */
	class GLWindow : public IWindow
	{
	public:
		GLWindow(const WindowDesc& desc);
		~GLWindow() override;

		/* ���£�����һ֡ */
		void OnUpdate() override;

		/* ��ȡ���/�߶� */
		[[nodiscard]] uint32_t GetWidth() const override { return m_WindowInfo.Descriptor.Width; }
		[[nodiscard]] uint32_t GetHeight() const override { return m_WindowInfo.Descriptor.Height; }

		/* ���ô�ֱͬ�� */
		[[nodiscard]] bool IsVSync() const override { return m_WindowInfo.Descriptor.IsVSync; }
		void SetVSync(bool enable) override;

		/* ��ȡGLFWwindow* */
		[[nodiscard]] void* GetNativeWindow() const override { return m_pGLFWWindow; }

		/* ������Ӧ�¼� */
		virtual void SetEventCallback(const EventCallbackFunc& callback) override { m_WindowInfo.CallBackFunc = callback; }

	private:
		/* �������ڣ����������ģ�����Ӧ�¼� */
		void Build(const WindowDesc& desc);
		/* ���ٴ��� */
		void Destroy();

		/* ������Ϣ */
		struct WindowInfo
		{
			WindowDesc Descriptor;			/* �������� */
			EventCallbackFunc CallBackFunc; /* �����¼��ص� */
		};

		/* GLFWwindowָ�� */
		GLFWwindow* m_pGLFWWindow{ nullptr };
		/* ������Ϣ*/
		WindowInfo m_WindowInfo{};
		/* ���������� */
		UniquePtr<IRenderContext> m_pRenderContext{ nullptr };
	};

}