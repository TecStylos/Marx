#include "CrossController.h"

namespace Marx
{
    std::set<int> CrossController::s_availIDs = { 0, 1, 2, 3 };

    bool CrossController::onUpdate()
    {
        static constexpr int btnConvTable[(uint32_t)ControllerButton::Count]
            = {
                GLFW_GAMEPAD_BUTTON_A,
                GLFW_GAMEPAD_BUTTON_B,
                GLFW_GAMEPAD_BUTTON_X,
                GLFW_GAMEPAD_BUTTON_Y,
                GLFW_GAMEPAD_BUTTON_START,
                GLFW_GAMEPAD_BUTTON_BACK,
                GLFW_GAMEPAD_BUTTON_DPAD_UP,
                GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
                GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
                GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
                GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
                GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
                GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
                GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
            };

        
        std::swap(m_pStateNew, m_pStateOld);
        if (!glfwGetGamepadState(m_uid, m_pStateNew))
        {
            if (m_isConnected)
            {
                m_isConnected = false;
                ControllerDisconnectEvent event(m_id);
                m_eventCallback(event);
            }
            return false;
        }

        if (!m_isConnected)
        {
            m_isConnected = true;
            ControllerConnectEvent event(m_id);
            m_eventCallback(event);
        }

        for (uint32_t i = 0; i < (uint32_t)ControllerButton::Count; ++i)
        {
            int platKey = btnConvTable[i];
            if (m_pStateNew->buttons[platKey] == GLFW_PRESS && m_pStateOld->buttons[platKey] == GLFW_RELEASE)
            {
                SET_FLAG(m_cState.button[i], ControllerButtonState_Down);
                ControllerButtonPressEvent event(m_id, (ControllerButton)i);
                m_eventCallback(event);
            }
            else if (m_pStateNew->buttons[platKey] == GLFW_RELEASE && m_pStateOld->buttons[platKey] == GLFW_PRESS)
            {
                SET_FLAG(m_cState.button[i], ControllerButtonState_Up);
                ControllerButtonReleaseEvent event(m_id, (ControllerButton)i);
                m_eventCallback(event);
            }
            else
            {
                ADD_FLAG(m_cState.button[i], ControllerButtonState_Repeat);
            }
        }

        for (uint32_t i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; ++i)
        {
            if (m_pStateNew->axes[i] == m_pStateOld->axes[i])
                continue;
            
            switch (i)
            {
            case GLFW_GAMEPAD_AXIS_LEFT_X:
            case GLFW_GAMEPAD_AXIS_LEFT_Y:
            {
                int num = i - GLFW_GAMEPAD_AXIS_LEFT_X;
                *((float*)&m_cState.stick[0] + num) = m_pStateNew->axes[i] * (num ? -1.0f : 1.0f);
                ControllerStickMoveEvent event(m_id, ControllerStick::Left, m_cState.stick[0]);
                m_eventCallback(event);
                break;
            }
            case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:
            {
                m_cState.trigger[0] = m_pStateNew->axes[i] * 0.5f + 0.5f;
                ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Left, m_cState.trigger[0]);
                m_eventCallback(event);
                break;
            }
            case GLFW_GAMEPAD_AXIS_RIGHT_X:
            case GLFW_GAMEPAD_AXIS_RIGHT_Y:
            {
                int num = i - GLFW_GAMEPAD_AXIS_RIGHT_X;
                *((float*)&m_cState.stick[1] + num) = m_pStateNew->axes[i] * (num ? -1.0f : 1.0f);
                ControllerStickMoveEvent event(m_id, ControllerStick::Right, m_cState.stick[1]);
                m_eventCallback(event);
                break;
            }
            case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:
            {
                m_cState.trigger[1] = m_pStateNew->axes[i] * 0.5f + 0.5f;
                ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Right, m_cState.trigger[1]);
                m_eventCallback(event);
                break;
            }
            }
        }

        return true;
    }
}