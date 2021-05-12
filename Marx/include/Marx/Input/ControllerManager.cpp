#include "mxpch.h"
#include "ControllerManager.h"

#ifdef MX_PLATFORM_WINDOWS
#include "Marx/Platform/XInput/XController.h"
#endif

namespace Marx
{
	bool ControllerManager::s_initialized = false;
	EventCallbackFunc ControllerManager::s_eventCallback;
	std::map<ControllerID, Reference<Controller>> ControllerManager::s_controllers;
	ControllerID ControllerManager::s_nextControllerID = 1;

	void ControllerManager::init(const EventCallbackFunc& callback)
	{
		MX_CORE_ASSERT(!s_initialized, "ControllerManager already initialized!");

		s_eventCallback = callback;

		s_initialized = true;
	}

	void ControllerManager::shutdown()
	{
		MX_CORE_ASSERT(s_initialized, "ControllerManager is not initialized!");

		s_initialized = false;
	}

	void ControllerManager::onUpdate()
	{
		for (auto& controller : s_controllers)
			controller.second->onUpdate();
	}
	const Reference<Controller> ControllerManager::getController(ControllerID id)
	{
		return s_controllers[id];
	}

	ControllerID ControllerManager::createController()
	{
		Reference<Controller> newController;
		ControllerID newControllerID = s_nextControllerID++;

		#if defined MX_PLATFORM_WINDOWS
		newController = std::make_shared<XController>(newControllerID);
		#elif defined MX_PLATFORM_UNIX
		// newController = std::make_shared<UnixController>(newControllerID);
		#endif

		newController->setEventCallback(s_eventCallback);

		s_controllers.insert(std::make_pair(newControllerID, newController));

		return newControllerID;
	}

	void ControllerManager::destroyController(ControllerID cid)
	{
		s_controllers.erase(cid);
	}
}
