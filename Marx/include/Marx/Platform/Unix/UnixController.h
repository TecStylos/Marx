#pragma once

#include <linux/joystick.h>
#include <set>

#include "Marx/Input/Controller.h"

namespace Marx
{
    class UnixController : public Controller
    {
    public:
        UnixController(ControllerID id)
            : Controller(id)
        {
            m_uid = *s_availIDs.begin();
            s_availIDs.erase(s_availIDs.begin());
            m_filepath.append(std::to_string(m_uid));
        }
    public:
        bool onUpdate() override;
        const ControllerButtonState& buttonState(ControllerButton btn) const override { return m_cState.button[(uint32_t)btn]; }
		const ControllerStickState& stickState(ControllerStick stick) const override { return m_cState.stick[(uint32_t)stick]; }
		float triggerState(ControllerTrigger trigger) const override { return m_cState.trigger[(uint32_t)trigger]; }
	public:
		ControllerType getType() const override { return ControllerType::Unix; }
    private:
        void processEvent();
    private:
        int m_uid = 0;
        int m_fd = 0;
        std::string m_filepath = "/dev/input/js";
        js_event m_event;
    private:
        static std::set<uint16_t> s_availIDs;
    };
}