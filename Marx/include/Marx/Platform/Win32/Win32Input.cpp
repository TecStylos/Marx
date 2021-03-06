#include "mxpch.h"
#include "Win32Input.h"

#include "Marx/Application.h"
#include "Marx/Input/KeyCodeConverter.h"

namespace Marx
{
	bool WindowsInput::isKeyPressedImpl(Key keyCode) const
	{
		SHORT state = GetKeyState(KeyCodeConverter::toWin32(keyCode));
		return (state >> 15);
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button) const
	{
		int convButton[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };
		SHORT state = GetKeyState(convButton[button]);
		return (state >> 15);
	}

	std::pair<float, float> WindowsInput::getMousePosImpl() const
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(static_cast<HWND>(Application::get()->getWindow()->getNativeWindow()), &p);
		return { (float)p.x, (float)p.y };
	}

	float WindowsInput::getMouseXImpl() const
	{
		auto [x, y] = getMousePosImpl();
		return x;
	}

	float WindowsInput::getMouseYImpl() const
	{
		auto [x, y] = getMousePosImpl();
		return y;
	}
}