#include "mxpch.h"
#include "UnixController.h"

#include <fcntl.h>

namespace Marx
{
    std::set<uint16_t> UnixController::s_availIDs = { 0, 1, 2, 3 };

    bool UnixController::onUpdate()
    {
        if (m_fd == 0) // Device file not open
        {
            m_fd = open(m_filepath.c_str(), O_NONBLOCK);
            if (m_fd < 0) // Unable to open device file
            {
                m_fd = 0;
                return false;
            }
            else
            {
                if (!m_isConnected)
                {
                    m_isConnected = true;
                    ControllerConnectEvent event(m_id);
                    m_eventCallback(event);
                }
            }
        }

        while (read(m_fd, &m_event, sizeof(m_event)) > 0)
        {
            processEvent();
        }
        if (errno != EAGAIN)
        {
            if (m_isConnected)
            {
                m_isConnected = false;
                ControllerDisconnectEvent event(m_id);
                m_eventCallback(event);
            }
            close(m_fd);
            m_fd = 0;
            return false;
        }

        return true; // Device still active
    }

    void UnixController::processEvent()
    {
        static constexpr ControllerButton convTable[(uint32_t)ControllerButton::Count]
            = {
                ControllerButton::A,
                ControllerButton::B,
                ControllerButton::X,
                ControllerButton::Y,
                ControllerButton::Shoulder_Left,
                ControllerButton::Shoulder_Right,
                ControllerButton::Back,
                ControllerButton::Start,
                ControllerButton::Back, // Logo button
                ControllerButton::Thumb_Left,
                ControllerButton::Thumb_Right
            };

        switch (m_event.type & ~JS_EVENT_INIT)
        {
        case JS_EVENT_AXIS:
        {
            ControllerButton btn = (ControllerButton)0;
            switch (m_event.number)
            {
            case 0: // Left thumb X
            case 1: // Left thumb Y
            {
                int num = m_event.number - 0;
                *((float*)&m_cState.stick[0] + num) = SHORT_TO_NORM_FLOAT(m_event.value) * (num ? -1.0f : 1.0f);
                ControllerStickMoveEvent event(m_id, ControllerStick::Left, m_cState.stick[0]);
                m_eventCallback(event);
                break;
            }
            case 2: // Left trigger
            {
                m_cState.trigger[0] = SHORT_TO_NORM_FLOAT(m_event.value) * 0.5f + 0.5f;
                ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Left, m_cState.trigger[0]);
                m_eventCallback(event);
                break;
            }
            case 3: // Right thumb X
            case 4: // Right thumb Y
            {
                int num = m_event.number - 3;
                *((float*)&m_cState.stick[1] + num) = SHORT_TO_NORM_FLOAT(m_event.value) * (num ? -1.0f : 1.0f);
                ControllerStickMoveEvent event(m_id, ControllerStick::Right, m_cState.stick[1]);
                m_eventCallback(event);
                break;
            }
            case 5: // Right trigger
            {
                m_cState.trigger[1] = SHORT_TO_NORM_FLOAT(m_event.value) * 0.5f + 0.5f;
                ControllerTriggerMoveEvent event(m_id, ControllerTrigger::Right, m_cState.trigger[1]);
                m_eventCallback(event);
                break;
            }
            case 6: // DPAD Left/Right
                if (m_event.value < 0) // Left
                    btn = ControllerButton::DPAD_Left;
                else if (m_event.value > 0) // Right
                    btn = ControllerButton::DPAD_Right;
            case 7: // DPAD Up/Down
                if (!(uint32_t)btn)
                {
                    if (m_event.value < 0) // Up
                        btn = ControllerButton::DPAD_Up;
                    else if (m_event.value > 0) // Down
                        btn = ControllerButton::DPAD_Down;
                }
                if ((uint32_t)btn)
                {
                    m_cState.button[(uint32_t)btn] = ControllerButtonState_Down;
                    ControllerButtonPressEvent event(m_id, btn);
                    m_eventCallback(event);
                }
                else
                {
                    for (uint32_t i = (uint32_t)ControllerButton::DPAD_Up; i <= (uint32_t)ControllerButton::DPAD_Right; ++i)
                    {
                        if (m_cState.button[i] & ControllerButtonState_Down)
                        {
                            m_cState.button[i] = ControllerButtonState_Up;
                            ControllerButtonReleaseEvent event(m_id, (ControllerButton)i);
                            m_eventCallback(event);
                        }
                    }
                }
                break;
            }
            break;
        }
        case JS_EVENT_BUTTON:
        {
            auto btn = convTable[m_event.number];
            if (m_event.value) // Button is pressed
            {
                SET_FLAG(m_cState.button[(uint32_t)btn], ControllerButtonState_Down);
                ControllerButtonPressEvent event(m_id, btn);
                m_eventCallback(event);
            }
            else // Button is released
            {
                SET_FLAG(m_cState.button[(uint32_t)btn], ControllerButtonState_Up);
                ControllerButtonReleaseEvent event(m_id, btn);
                m_eventCallback(event);
            }
            break;
        }
        }
    }
}