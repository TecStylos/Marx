#pragma once

#include "Marx/Core/Profiler.h"
#include "Marx/Exceptions/MarxException.h"
#include "Marx/Renderer/RendererAPISelector.h"
#include "Marx/WindowAPISelector.h"
#include "Marx/Input/KeyCodeConverter.h"

extern Marx::Application* Marx::createApplication();

#define MX_EXCEPT_LOG(except) MX_CRITICAL(except.what())
#define MX_STD_EXCEPT_LOG(except) MX_CRITICAL(except.what())

int main(int argc, char* argv[], char* env[])
{
	MX_PROFILER_BEGIN_SESSION("results.json");
	{
		MX_PROFILE_SCOPE("API Selection");

		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			if ("--supported-renderer-apis" == arg)
			{
				Marx::printSupportedRendererAPIs();
				exit(0);
			}
			else if ("--supported-window-apis" == arg)
			{
				Marx::printSupportedWindowAPIs();
				exit(0);
			}
		}
		Marx::selectRendererAPIFromCmdLine(argc, argv);
		Marx::selectWindowAPIFromCmdLine(argc, argv);
	}

	{
		{
			MX_PROFILE_SCOPE("Initialization");

			Marx::Log::init();
			MX_CORE_INFO("Initialized Log!");

			auto wndAPI = Marx::Window::getAPI();
			auto rndAPI = Marx::RendererAPI::getAPI();

			MX_CORE_ASSERT(
				!(
					(wndAPI == Marx::Window::API::GLFW && rndAPI == Marx::RendererAPI::API::D3D11) ||
					(wndAPI == Marx::Window::API::Win32 && rndAPI == Marx::RendererAPI::API::OpenGL)
					),
				"The selected API combination is not supported!"
			);

			Marx::Input::init();
			MX_CORE_INFO("Initialized input!");

			Marx::KeyCodeConverter::init();
			MX_CORE_INFO("Initialized keycode converter!");
		}

		Marx::Scope<Marx::Application> app;

		try
		{
			{
				MX_PROFILE_SCOPE("Application Creation");
				app.reset(Marx::createApplication());
			}
			{
				MX_PROFILE_SCOPE("Application Run");
				app->run();
			}
		}
		catch (const Marx::MarxException& except) { MX_EXCEPT_LOG(except); MX_DEBUG_BREAK(); }
		catch (const std::exception& except) { MX_STD_EXCEPT_LOG(except); MX_DEBUG_BREAK(); }
	}
}