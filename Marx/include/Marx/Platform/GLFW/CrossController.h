#pragma once

#include "Marx/Input/Controller.h"
#include <set>

#include "GLFW/glfw3.h"

namespace Marx
{

    class CrossController : public Controller
    {
    public:
        CrossController(ControllerID id)
            : Controller(id)
        {
            m_uid = *s_availIDs.begin();
            s_availIDs.erase(s_availIDs.begin());
        }
    public:
        bool onUpdate() override;
    public:
        ControllerType getType() const override { return ControllerType::GLFW; }
    private:
        int m_uid;
        GLFWgamepadstate m_states[2] = {};
        GLFWgamepadstate* m_pStateNew = &m_states[0];
        GLFWgamepadstate* m_pStateOld = &m_states[1];
    private:
        static std::set<int> s_availIDs;
    };
}