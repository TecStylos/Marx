#include "mxpch.h"
#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

namespace Marx
{
	std::vector<OpenGLGraphicsContext*> OpenGLGraphicsContext::s_contexts;

	OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* wnd)
		: m_wnd(wnd)
	{
		MX_CORE_ASSERT(wnd, "Window handle is null!");

		s_contexts.push_back(this);
	}

	OpenGLGraphicsContext::~OpenGLGraphicsContext()
	{
		for (uint32_t i = 0; i < s_contexts.size(); ++i)
		{
			if (s_contexts[i] == this)
			{
				s_contexts.erase(s_contexts.begin() + i);
				break;
			}
		}
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
		; // Nothing to do here, see OpenGLRendererAPI
	}

	void OpenGLGraphicsContext::swapBuffers()
	{
		glfwSwapBuffers(m_wnd);
	}

	void OpenGLGraphicsContext::onResize(uint32_t width, uint32_t height)
	{
		; // Nothing to do here
	}

	void OpenGLGraphicsContext::enableDepthTest(bool enabled)
	{
		if (enabled)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLGraphicsContext::enableBlending(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void OpenGLGraphicsContext::enableVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);
	}
}