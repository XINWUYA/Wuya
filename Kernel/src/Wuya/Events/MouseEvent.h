#pragma once
#include "Event.h"
#include <Wuya/Core/MouseCodes.h>

namespace Wuya
{
	/*
	* ����ƶ��¼���
	*/
	class MouseMovedEvent : public IEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseMoved)
			
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y) 
		{}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }

	private:
		float m_MouseX, m_MouseY;
	};

	/*
	* �������¼���
	*/
	class MouseScrolledEvent : public IEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseScrolled)
			
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) 
		{}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput; }

	private:
		float m_XOffset, m_YOffset;
	};

	/*
	* ��갴���¼���
	*/
	class MouseButtonEvent : public IEvent
	{
	public:
		MouseCode GetMouseButton() const { return m_Button; }

		virtual int GetCategoryFlags() const override { return EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton; }

	protected:
		MouseButtonEvent(const MouseCode button)
			: m_Button(button) 
		{}

		MouseCode m_Button;
	};

	/*
	* ��갴�������¼���
	*/
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonPressed)
			
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button) 
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
	};

	/*
	* ��갴���ͷ��¼���
	*/
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		EVENT_CLASS_TYPE(MouseButtonReleased)
			
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button) 
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}
	};
}