#pragma once

#include <map>

#include "Controller.h"

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
		static const Reference<Controller> getController(ControllerID id);
		static ControllerID createController();
		static void destroyController(ControllerID cid);
	private:
		static bool s_initialized;
		DISABLE_DLL_INTERFACE_WARN;
		static EventCallbackFunc s_eventCallback;
		REENABLE_DLL_INTERFACE_WARN;
		static std::map<ControllerID, Reference<Controller>> s_controllers;
		static ControllerID s_nextControllerID;
	};
}