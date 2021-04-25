#include "mxpch.h"
#include "Window.h"

#ifdef MX_ENABLE_WIN32
   #include "Platform/Win32/Win32Window.h"
#endif

#ifdef MX_ENABLE_GLFW
   #include "Platform/GLFW/CrossWindow.h"
#endif

namespace Marx
{
	Window::API Window::s_api = Window::API::None;

	Window* Window::create(const WindowDesc& desc)
	{
		switch (getAPI())
		{
		case API::None:
			MX_CORE_ASSERT(false, "A window API must be selected before calling Window::create()!");
			break;
		case API::Win32:
			#ifdef MX_ENABLE_WIN32
			return new Win32Window(desc);
			#else
			MX_CORE_ASSERT(false, "The window API 'Win32' is not supported by this configuration!");
			#endif
			break;
		case API::GLFW:
			#ifdef MX_ENABLE_GLFW
			return new CrossWindow(desc);
			#else
			MX_CORE_ASSERT(false, "The window API 'GLFW' is not supported by this configuration!");
			#endif
			break;
		default:
			MX_CORE_ASSERT(false, "The selected window API is not supported!");
			break;
		}

		return nullptr;
	}
}