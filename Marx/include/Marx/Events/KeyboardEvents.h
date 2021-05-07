#pragma once

#include "Event.h"

#include "Marx/Input/KeyCodes.h"

namespace Marx
{

	class MARX_API KeyEvent : public Event
	{
	public:
		inline Key getKeyCode() const { return m_keyCode; }

		EVENT_CLASS_FLAGS(EventFlagInput | EventFlagKeyboard)
	protected:
		KeyEvent(Window* pWnd, Key keyCode)
			: Event(pWnd), m_keyCode(keyCode)
		{}
	protected:
		Key m_keyCode;
	};

	class MARX_API KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(Window* pWnd, Key keyCode, int repeatCount)
			: KeyEvent(pWnd, keyCode), m_repeatCount(repeatCount)
		{}
	public:
		int getRepeatCount() const { return m_repeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPress: " << (int)m_keyCode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPress)
	protected:
		int m_repeatCount;
	};

	class MARX_API KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(Window* pWnd, Key keyCode)
			: KeyEvent(pWnd, keyCode)
		{}
	public:
		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyRelease: " << (int)m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyRelease)
	};

	class MARX_API KeyTypeEvent : public KeyEvent
	{
	public:
		KeyTypeEvent(Window* pWnd, Key keyCode)
			: KeyEvent(pWnd, keyCode)
		{}
	public:
		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyType: " << (int)m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyType)
	};
}