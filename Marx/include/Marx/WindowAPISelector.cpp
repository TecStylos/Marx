#include "mxpch.h"
#include "WindowAPISelector.h"

#include "Window.h"

namespace Marx
{
	void selectWindowAPIFromCmdLine(int argc, char* argv[])
	{
		#if defined MX_ENABLE_WIN32
		Window::setAPI(Window::API::Win32);
		#elif defined MX_ENABLE_GLFW
		Window::setAPI(Window::API::GLFW);
		#endif

		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];
			if ("--win32" == arg)
				Window::setAPI(Window::API::Win32);
			else if ("--glfw" == arg)
				Window::setAPI(Window::API::GLFW);
		}
	}

	void printSupportedWindowAPIs()
	{
		std::cout << "Supported APIs: ";
		#ifdef MX_ENABLE_WIN32
		std::cout << "Win32 ";
		#endif
		#ifdef MX_ENABLE_GLFW
		std::cout << "GLFW ";
		#endif
	}
}