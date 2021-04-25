#include "mxpch.h"
#include "CrossWindow.h"

#include "Marx/Events/WindowEvents.h"
#include "Marx/Events/KeyboardEvents.h"
#include "Marx/Events/MouseEvents.h"

#include "Marx/Platform/OpenGL/OpenGLGraphicsContext.h"

namespace Marx
{
	static bool s_glfwInitialized = false;

	CrossWindow::CrossWindow(const WindowDesc& desc)
	{
		init(desc);
	}

	CrossWindow::~CrossWindow()
	{
		shutdown();
	}

	void CrossWindow::onUpdate()
	{
		glfwPollEvents();
		if (!m_data.wndClosed)
			m_pContext->swapBuffers();
	}

	void CrossWindow::init(const WindowDesc& desc)
	{
		m_data.title = desc.title;
		m_data.width = desc.width;
		m_data.height = desc.height;
		m_data.pWnd = this;

		MX_CORE_INFO("Creating window '{0}' ({1}, {2})", m_data.title, m_data.width, m_data.height);
	
		if (!s_glfwInitialized)
		{
			MX_CORE_ASSERT(glfwInit(), "Could not initialize GLFW!");
			glfwSetErrorCallback(
				[](int error, const char* description)
				{
					MX_CORE_ERROR("GLFW Error [{0}]: {1}", error, description);
				}
			);
			s_glfwInitialized = true;
		}

		m_wnd = glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);

		m_pContext = new OpenGLGraphicsContext(m_wnd);
		m_pContext->init();

		glfwSetWindowUserPointer(m_wnd, &m_data);
		glfwSwapInterval(1);

		glfwSetWindowSizeCallback(
			m_wnd,
			[](GLFWwindow* wnd, int width, int height)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);
				data.width = width;
				data.height = height;
				data.eventCallback(WindowResizeEvent(data.pWnd, width, height));
			}
		);

		glfwSetWindowCloseCallback(
			m_wnd,
			[](GLFWwindow* wnd)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);
				data.eventCallback(WindowCloseEvent(data.pWnd));
				data.wndClosed = true;
			}
		);

		glfwSetKeyCallback(
			m_wnd,
			[](GLFWwindow* wnd, int key, int scancode, int action, int mods)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);

				switch (action)
				{
				case GLFW_PRESS:
					data.eventCallback(KeyPressEvent(data.pWnd, key, 0));
					break;
				case GLFW_RELEASE:
					data.eventCallback(KeyReleaseEvent(data.pWnd, key));
					break;
				case GLFW_REPEAT:
					data.eventCallback(KeyPressEvent(data.pWnd, key, 1));
					break;
				}
			}
		);

		glfwSetCharCallback(
			m_wnd,
			[](GLFWwindow* wnd, unsigned int key)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);
				data.eventCallback(KeyTypeEvent(data.pWnd, key));
			}
		);

		glfwSetMouseButtonCallback(
			m_wnd,
			[](GLFWwindow* wnd, int btn, int action, int mods)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);

				switch (action)
				{
				case GLFW_PRESS:
					data.eventCallback(MouseButtonPressEvent(data.pWnd, btn));
					break;
				case GLFW_RELEASE:
					data.eventCallback(MouseButtonReleaseEvent(data.pWnd, btn));
					break;
				}
			}
		);

		glfwSetScrollCallback(
			m_wnd,
			[](GLFWwindow* wnd, double xOffset, double yOffset)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);
				if (xOffset != 0.0)
					data.eventCallback(MouseScrollEvent(data.pWnd, (float)xOffset));
				if (yOffset != 0.0)
					data.eventCallback(MouseHScrollEvent(data.pWnd, (float)yOffset));
			}
		);

		glfwSetCursorPosCallback(
			m_wnd,
			[](GLFWwindow* wnd, double xPos, double yPos)
			{
				WndData& data = *(WndData*)glfwGetWindowUserPointer(wnd);
				data.eventCallback(MouseMoveEvent(data.pWnd, (float)xPos, (float)yPos));
			}
		);
	}

	void CrossWindow::shutdown()
	{
		if (!m_data.wndClosed)
			glfwDestroyWindow(m_wnd);
	}
}