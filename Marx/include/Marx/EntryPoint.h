#pragma once

#include "Marx/Exceptions/MarxException.h"
#include "Marx/Renderer/RendererAPISelector.h"

extern Marx::Application* Marx::createApplication();

#define MX_EXCEPT_LOG(except) MX_CRITICAL(except.what())
#define MX_STD_EXCEPT_LOG(except) MX_CRITICAL(except.what())

int main(int argc, char* argv[], char* env[])
{
	Marx::selectRendererAPIFromCmdLine(argc, argv);

	{
		Marx::Log::init();
		MX_CORE_INFO("Initialized Log!");

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