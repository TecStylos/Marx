#include "mxpch.h"
#include "XController.h"

namespace Marx
{
	std::set<DWORD> XController::s_availUIDs = { 0, 1, 2, 3 };

	bool XController::onUpdate()
	{
		static constexpr int convTable[(uint32_t)ControllerButton::Count]
			= {
			0x1000, 0x2000, 0x4000, 0x8000,
			0x0010, 0x0020,
			0x0001, 0x0002, 0x0004, 0x0008,
			0x0040, 0x0080, 0x0100, 0x0200
		};

		std::swap(m_pStateNew, m_pStateOld);

		if (XInputGetState(m_uid, m_pStateNew) == ERROR_DEVICE_NOT_CONNECTED)
		{
			if (m_isConnected)
			{
				m_isConnected = false;
				ControllerDisconnectEvent event(m_id);
				m_eventCallback(event);
			}
			return false; // Device disconnected, can be removed from pool
		}
		
		if (!m_isConnected)
		{
			m_isConnected = true;
			ControllerConnectEvent event(m_id);
			m_eventCallback(event);
		}

		if (m_pStateNew->dwPacketNumber != m_pStateOld->dwPacketNumber) // Device state has changed
		{
			XINPUT_GAMEPAD& gpNew = m_pStateNew->Gamepad;
			XINPUT_GAMEPAD& gpOld = m_pStateOld->Gamepad;

			// Get changed buttons
			WORD diff = gpNew.wButtons ^ gpOld.wButtons;

			// Loop over all buttons and update them appropiately
			for (int i = 0; i < (uint32_t)ControllerButton::Count; ++i)
			{
				if (HAS_FLAG(diff, convTable[i])) // Button state changed
				{
					if (HAS_FLAG(gpNew.wButtons, convTable[i])) // Button is pressed
					{
						SET_FLAG(m_cState.button[i], ControllerButtonState_Down);
						ControllerButtonPressEvent event(m_id, (ControllerButton)i);
						m_eventCallback(event);
					}
					else // Button is released
					{
						SET_FLAG(m_cState.button[i], ControllerButtonState_Up);
						ControllerButtonReleaseEvent event(m_id, (ControllerButton)i);
						m_eventCallback(event);
					}
				}
				else // No state change
				{
					// Update state to repeat
					ADD_FLAG(m_cState.button[i], ControllerButtonState_Repeat);
				}
			}

			float triggerDelta;
			triggerDelta = calcTriggerDZ(BYTE_TO_NORM_FLOAT(gpNew.bLeftTrigger));
			if (triggerDelta != m_cState.trigger[(uint32_t)ControllerTrigger::Left]) // Left trigger moved
			{
				m_cState.trigger[(uint32_t)ControllerTrigger::Left] = triggerDelta;
				ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Left, m_cState.trigger[(uint32_t)ControllerTrigger::Left]);
				m_eventCallback(event);
			}
			triggerDelta = calcTriggerDZ(BYTE_TO_NORM_FLOAT(gpNew.bRightTrigger));
			if (triggerDelta != m_cState.trigger[(uint32_t)ControllerTrigger::Right]) // Right trigger moved
			{
				m_cState.trigger[(uint32_t)ControllerTrigger::Right] = triggerDelta;
				ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Right, m_cState.trigger[(uint32_t)ControllerTrigger::Right]);
				m_eventCallback(event);
			}

			ControllerStickState stickState;
			stickState = calcStickDZ({ SHORT_TO_NORM_FLOAT(gpNew.sThumbLX), SHORT_TO_NORM_FLOAT(gpNew.sThumbLY) });
			if (stickState != m_cState.stick[(uint32_t)ControllerStick::Left]) // Left stick moved
			{
				m_cState.stick[(uint32_t)ControllerStick::Left] = stickState;
				ControllerStickMoveEvent event(m_id, ControllerStick::Left, m_cState.stick[(uint32_t)ControllerStick::Left]);
				m_eventCallback(event);
			}
			stickState = calcStickDZ({ SHORT_TO_NORM_FLOAT(gpNew.sThumbRX), SHORT_TO_NORM_FLOAT(gpNew.sThumbRY) });
			if (stickState != m_cState.stick[(uint32_t)ControllerStick::Right]) // Left stick moved
			{
				m_cState.stick[(uint32_t)ControllerStick::Right] = stickState;
				ControllerStickMoveEvent event(m_id, ControllerStick::Right, m_cState.stick[(uint32_t)ControllerStick::Right]);
				m_eventCallback(event);
			}
		}
		else // Device state has not changed
		{
			// Set all buttons to repeat
			for (int i = 0; i < (uint32_t)ControllerButton::Count; ++i)
			{
				ADD_FLAG(m_cState.button[i], ControllerButtonState_Repeat);
			}
		}

		return true; // Device still active
	}
}