#pragma once
#include "Wuya/Events/Event.h"

namespace Wuya
{
	struct WindowConfig
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool IsVSync;

		WindowConfig(const std::string& title = "Wuya", uint32_t width = 1280, uint32_t height = 720, bool is_vsync = true)
			: Title(title), Width(width), Height(height), IsVSync(is_vsync)
		{}
	};

	class IWindow
	{
	public:
		using EventCallbackFunc = std::function<void(IEvent*)>;

		virtual ~IWindow() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enable) = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;
		
		static UniquePtr<IWindow> Create(const WindowConfig& config = WindowConfig());

	protected:
		IWindow() = default;
	};
}
