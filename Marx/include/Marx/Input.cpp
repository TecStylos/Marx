#include "mxpch.h"
#include "Input.h"

#ifdef MX_ENABLE_WIN32
#include "Marx/Platform/Win32/Win32Input.h"
#endif

#ifdef MX_ENABLE_GLFW
#include "Marx/Platform/GLFW/CrossInput.h"
#endif

#include "Marx/Window.h"

namespace Marx{
    Input* Input::s_pInstance = nullptr;

    void Input::init()
    {
        switch(Window::getAPI())
        {
        case Window::API::None:
            MX_CORE_ASSERT(false, "A window API must be selected before calling Input::init()!");
            return;
        case Window::API::Win32:
            #ifdef MX_ENABLE_WIN32
            s_pInstance = new Win32Input();
            #else
            MX_CORE_ASSERT(false, "Win32Input is not supported!");
            #endif
            return;
        case Window::API::GLFW:
            #ifdef MX_ENABLE_GLFW
            s_pInstance = new CrossInput();
            #else
            MX_CORE_ASSERT(false, "CrossInput is not supporte!");
            #endif
            return;
        }

        MX_CORE_ASSERT(false, "Unknown window API!");
    }
}