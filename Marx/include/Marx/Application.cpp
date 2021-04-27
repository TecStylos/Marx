#include "mxpch.h"

#include "Application.h"

#include "Marx/Core/Log.h"
#include "Marx/Renderer/Renderer.h"
#include "Marx/Input/ControllerManager.h"

#include "Marx/Input.h"

namespace Marx
{
	Application* Application::s_instance = nullptr;

	Application::Application()
		: m_timestep(Timestep::secondsSinceProgramStartup())
	{
		MX_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_pWindow.reset(Window::create());
		m_pWindow->setEventCallback(MX_BIND_EVENT_METHOD(Application::onEvent));

		Renderer::init();

		m_pImGuiLayer = new ImGuiLayer;
		pushOverlay(m_pImGuiLayer);

		ControllerManager::init(MX_BIND_EVENT_METHOD(Application::onEvent));
	}

	Application::~Application()
	{
		ControllerManager::shutdown();
	}

	void Application::run()
	{
		static float rotation = 0.0f;

		while (m_running)
		{
			m_timestep.update(Timestep::secondsSinceProgramStartup());

			if (!m_isMinimized)
			{
				for (Layer* layer : m_layerStack)
					layer->onUpdate(m_timestep);
			}

			m_pImGuiLayer->begin();
			for (Layer* layer : m_layerStack)
				layer->onImGuiRender();
			m_pImGuiLayer->end();

			m_pWindow->onUpdate();

			ControllerManager::onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(MX_BIND_EVENT_METHOD(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(MX_BIND_EVENT_METHOD(Application::onWindowResize));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin(); )
		{
			(*--it)->onEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		m_layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay)
	{
		m_layerStack.pushOverlay(overlay);
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		e.getWnd()->shutdown();
		if (e.getWnd()->getWndCount() == 0)
		{
			m_running = false;
		}
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_isMinimized = true;
			return false;
		}

		m_isMinimized = false;

		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}
}