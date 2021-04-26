#include "mxpch.h"
#include "ControllerManager.h"

namespace Marx
{
	bool ControllerManager::s_initialized = false;
	EventCallbackFunc ControllerManager::s_eventCallback;
	Reference<Controller> ControllerManager::s_controller[MX_USER_MAX_COUNT];

	void ControllerManager::init(const EventCallbackFunc& callback)
	{
		MX_CORE_ASSERT(!s_initialized, "ControllerManager already initialized!");

		s_eventCallback = callback;

		for (int i = 0; i < MX_USER_MAX_COUNT; ++i)
		{
			#ifdef MX_PLATFORM_WINDOWS
				s_controller[i] = std::make_shared<XController>(i, i);
				s_controller[i]->setEventCallback(s_eventCallback);
			#endif
		}

		s_initialized = true;
	}

	void ControllerManager::shutdown()
	{
		MX_CORE_ASSERT(s_initialized, "ControllerManager is not initialized!");

		s_initialized = false;
	}

	void ControllerManager::onUpdate()
	{
		for (int i = 0; i < MX_USER_MAX_COUNT; ++i)
		{
			#ifdef MX_PLATFORM_WINDOWS
			s_controller[i]->onUpdate();
			#endif
		}
	}
	const Controller& ControllerManager::getController(ControllerID id)
	{
		return *s_controller[id];
	}
}
