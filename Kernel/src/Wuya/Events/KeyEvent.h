#pragma once
#include "Event.h"
#include "Wuya/Core/KeyCodes.h"

namespace Wuya
{
	/*
	* �����¼���
	*/
	class KeyEvent: public IEvent
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }

	protected:
		KeyEvent(const KeyCode keycode)
			: m_KeyCode(keycode) 
		{}

		KeyCode m_KeyCode;
	};

	/*
	* ���������¼���
	*/
	class KeyPressedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyPressed)

		KeyPressedEvent(const KeyCode keycode, const uint16_t repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) 
		{}

		uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

	private:
		uint16_t m_RepeatCount;
	};

	/*
	* �����ͷ��¼���
	*/
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyReleased)
			
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) 
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}
	};

	/*
	* ���������¼���
	*/
	class KeyTypedEvent : public KeyEvent
	{
	public:
		EVENT_CLASS_TYPE(KeyTyped)
			
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) 
		{}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}
	};
}