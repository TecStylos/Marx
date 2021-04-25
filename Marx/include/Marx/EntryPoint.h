#pragma once

#include "Marx/Exceptions/MarxException.h"
#include "Marx/Renderer/RendererAPISelector.h"
#include "Marx/WindowAPISelector.h"

extern Marx::Application* Marx::createApplication();

#define MX_EXCEPT_LOG(except) MX_CRITICAL(except.what())
#define MX_STD_EXCEPT_LOG(except) MX_CRITICAL(except.what())

int main(int argc, char* argv[], char* env[])
{
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

	auto wndAPI = Marx::Window::getAPI();
	auto rndAPI = Marx::RendererAPI::getAPI();

	{
		Marx::Log::init();
		MX_CORE_INFO("Initialized Log!");

		MX_CORE_ASSERT(
			!(
				(wndAPI == Marx::Window::API::GLFW && rndAPI == Marx::RendererAPI::API::D3D11) ||
				(wndAPI == Marx::Window::API::Win32 && rndAPI == Marx::RendererAPI::API::OpenGL)
				),
			"The selected API combination is not supported!"
		);

		Marx::Scope<Marx::Application> app;

		try
		{
			app.reset(Marx::createApplication());
			app->run();
		}
		catch (const Marx::MarxException& except) { MX_EXCEPT_LOG(except); MX_DEBUG_BREAK(); }
		catch (const std::exception& except) { MX_STD_EXCEPT_LOG(except); MX_DEBUG_BREAK(); }
	}
}