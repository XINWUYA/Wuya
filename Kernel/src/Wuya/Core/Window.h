#pragma once
#include "Wuya/Events/Event.h"

namespace Wuya
{
	/* �������� */
	struct WindowDesc
	{
		std::string Title;	/* ���ڱ��� */
		uint32_t Width;		/* ���ڿ�� */
		uint32_t Height;	/* ���ڸߵ� */
		bool IsVSync;		/* ��ֱͬ�� */

		WindowDesc(std::string title = "Wuya", uint32_t width = 1920, uint32_t height = 1080, bool is_vsync = false)
			: Title(std::move(title)), Width(width), Height(height), IsVSync(is_vsync)
		{}
	};

	/* �����࣬���ڴ����༭���������� */
	class IWindow
	{
	public:
		using EventCallbackFunc = std::function<void(IEvent*)>;

		virtual ~IWindow() = default;

		/* ���£�����һ֡ */
		virtual void OnUpdate() = 0;

		/* ��ȡ���/�߶� */
		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		/* ���ô�ֱͬ�� */
		[[nodiscard]] virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enable) = 0;

		/* ��ȡ����ʵ�壬��ͨ��GLFWwindow�����ģ��򷵻�GLFWwindow* */
		[[nodiscard]] virtual void* GetNativeWindow() const = 0;

		/* ������Ӧ�¼� */
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;

		/* ���������������� */
		static UniquePtr<IWindow> Create(const WindowDesc& desc = {});

	protected:
		IWindow() = default;
	};
}
