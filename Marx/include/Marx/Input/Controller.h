#pragma once

#include "Marx/Events/ControllerEvents.h"

namespace Marx
{
	#define BYTE_TO_NORM_FLOAT(b) (((float)b) / 255)
	#define SHORT_TO_NORM_FLOAT(s) (((float)s) / 32767)

	enum class ControllerType
	{
		None = 0,
		XInput,
		DInput,
		Unix
	};

	class MARX_API Controller
	{
	public:
		Controller(ControllerID id)
			: m_id(id), m_cState()
		{}
	public:
		ControllerID getID() const { return m_id; }
		void setEventCallback(const EventCallbackFunc& callback) const { m_eventCallback = callback; }
		const ControllerButtonState& buttonState(ControllerButton btn) const { return m_cState.button[(uint32_t)btn]; }
		const ControllerStickState& stickState(ControllerStick stick) const { return m_cState.stick[(uint32_t)stick]; }
		float triggerState(ControllerTrigger trigger) const { return m_cState.trigger[(uint32_t)trigger]; }
	public:
		// Returns zero when the controller is not connected.
		virtual bool onUpdate() = 0;
	public:
		virtual ControllerType getType() const = 0;
	protected:
		ControllerID m_id;
		bool m_isConnected = false;
		DISABLE_DLL_INTERFACE_WARN;
		mutable EventCallbackFunc m_eventCallback;
		REENABLE_DLL_INTERFACE_WARN;
	protected:
		struct ControllerState
		{
			ControllerButtonState button[(uint32_t)ControllerButton::Count];
			float trigger[2];
			ControllerStickState stick[2];
		} m_cState;
	};
}