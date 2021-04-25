#include "mxpch.h"
#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>

namespace Marx
{
	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* wnd)
		: m_wnd(wnd)
	{
		MX_CORE_ASSERT(wnd, "Window handle is null!");
	}

	void OpenGLGraphicsContext::init()
	{
		glfwMakeContextCurrent(m_wnd);
		MX_CORE_ASSERT(
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress),
			"Failed to initialize GLAD!"
		);

		MX_CORE_INFO("OpenGL Renderer: {0}", glGetString(GL_RENDERER));
		MX_CORE_INFO("       Vendor:   {0}", glGetString(GL_VENDOR));
		MX_CORE_INFO("       Version:  {0}", glGetString(GL_VERSION));
	}

	void OpenGLGraphicsContext::shutdown()
	{
		; // Nothing to do yet
	}

	void OpenGLGraphicsContext::clear(float color[4])
	{
		;
	}

	void OpenGLGraphicsContext::swapBuffers()
	{
		glfwSwapBuffers(m_wnd);
	}

	void OpenGLGraphicsContext::onResize(uint32_t width, uint32_t height)
	{
		;
	}

	void OpenGLGraphicsContext::enableDepthTest(bool enabled)
	{
		;
	}

	void OpenGLGraphicsContext::enableBlending(bool enabled)
	{
		;
	}

	void OpenGLGraphicsContext::enableVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}
}