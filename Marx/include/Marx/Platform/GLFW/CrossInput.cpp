#include "mxpch.h"
#include "CrossInput.h"

#include "Marx/Application.h"
#include "Marx/Input/KeyCodeConverter.h"
#include <GLFW/glfw3.h>

namespace Marx
{
    bool CrossInput::isKeyPressedImpl(Key keyCode) const
    {
        auto window = (GLFWwindow*)Application::get()->getWindow()->getNativeWindow();
        auto state = glfwGetKey(window, KeyCodeConverter::toGlfw(keyCode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool CrossInput::isMouseButtonPressedImpl(int button) const
    {
        auto window = (GLFWwindow*)Application::get()->getWindow()->getNativeWindow();
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> CrossInput::getMousePosImpl() const
    {
        auto window = (GLFWwindow*)Application::get()->getWindow()->getNativeWindow();
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        return { (float)xPos, (float)yPos };
    }

    float CrossInput::getMouseXImpl() const
    {
        auto[x, y] = getMousePosImpl();
        return x;
    }

    float CrossInput::getMouseYImpl() const
    {
        auto[x, y] = getMousePosImpl();
        return y;
    }
}