#pragma once
#include "Wuya/Events/Event.h"

namespace Wuya
{
	/* 窗口描述 */
	struct WindowDesc
	{
		std::string Title;	/* 窗口标题 */
		uint32_t Width;		/* 窗口宽度 */
		uint32_t Height;	/* 窗口高低 */
		bool IsVSync;		/* 垂直同步 */

		WindowDesc(std::string title = "Wuya", uint32_t width = 1920, uint32_t height = 1080, bool is_vsync = false)
			: Title(std::move(title)), Width(width), Height(height), IsVSync(is_vsync)
		{}
	};

	/* 窗口类，用于创建编辑器的主窗口 */
	class IWindow
	{
	public:
		using EventCallbackFunc = std::function<void(IEvent*)>;

		virtual ~IWindow() = default;

		/* 更新，交换一帧 */
		virtual void OnUpdate() = 0;

		/* 获取宽度/高度 */
		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;

		/* 设置垂直同步 */
		[[nodiscard]] virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enable) = 0;

		/* 获取窗口实体，如通过GLFWwindow创建的，则返回GLFWwindow* */
		[[nodiscard]] virtual void* GetNativeWindow() const = 0;

		/* 设置响应事件 */
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;

		/* 根据描述创建窗口 */
		static UniquePtr<IWindow> Create(const WindowDesc& desc = {});

	protected:
		IWindow() = default;
	};
}
