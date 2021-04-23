#pragma once

#include "Controller.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Marx/Platform/XInput/XController.h"
#endif

namespace Marx
{
	constexpr int MX_USER_MAX_COUNT = 4;

	class MARX_API ControllerManager
	{
	public:
		static void init(const EventCallbackFunc& callback);
		static void shutdown();
		static void onUpdate();
	public:
		static const Controller& getController(ControllerID id);
	private:
		static bool s_initialized;
		DISABLE_DLL_INTERFACE_WARN;
		static EventCallbackFunc s_eventCallback;
		REENABLE_DLL_INTERFACE_WARN;
		static Reference<Controller> s_controller[MX_USER_MAX_COUNT];
	};
}