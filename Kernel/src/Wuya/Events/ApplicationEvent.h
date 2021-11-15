#pragma once
#include "Event.h"

namespace Wuya
{
	/*
	* Ӧ���¼���
	*/
	class AppEvent : public IEvent
	{
	public:
		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

	protected:
		AppEvent() = default;
	};

	/*
	* ���ڹر��¼���
	*/
	class WindowCloseEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(WindowClose)
			
		WindowCloseEvent() = default;
	};

	/*
	* ����Resize�¼���
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
	* ���ھ۽��¼��� todo:
	*/
	class WindowFocusEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(WindowFocus)

		WindowFocusEvent() = default;
	};

	/*
	* Ӧ��Tick�¼���
	*/
	class AppTickEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppTick)
			
		AppTickEvent() = default;
	};

	/*
	* Ӧ�ø����¼���
	*/
	class AppUpdateEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppUpdate)
			
		AppUpdateEvent() = default;
	};

	/*
	* Ӧ����Ⱦ�¼���
	*/
	class AppRenderEvent : public AppEvent
	{
	public:
		EVENT_CLASS_TYPE(AppRender)
			
		AppRenderEvent() = default;
	};
}