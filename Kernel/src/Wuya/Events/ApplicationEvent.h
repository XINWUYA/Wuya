#pragma once
#include "Event.h"

namespace Wuya
{
	/*
	* 应用事件类
	*/
	class AppEvent : public IEvent
	{
	public:
		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

	protected:
		AppEvent() = default;
	};

	/*
	* 窗口关闭事件类
	*/
	class WindowCloseEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(WindowClose)
			
		WindowCloseEvent() = default;
	};

	/*
	* 窗口Resize事件类
	*/
	class WindowResizeEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(WindowResize)
			
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) 
		{}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

	private:
		unsigned int m_Width, m_Height;
	};

	/*
	* 窗口聚焦事件类 todo:
	*/
	class WindowFocusEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(WindowFocus)

		WindowFocusEvent() = default;
	};

	/*
	* 应用Tick事件类
	*/
	class AppTickEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppTick)
			
		AppTickEvent() = default;
	};

	/*
	* 应用更新事件类
	*/
	class AppUpdateEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppUpdate)
			
		AppUpdateEvent() = default;
	};

	/*
	* 应用渲染事件类
	*/
	class AppRenderEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppRender)
			
		AppRenderEvent() = default;
	};
}