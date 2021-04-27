#include "mxpch.h"
#include "OpenGLRendererAPI.h"

#include "OpenGLGraphicsContext.h"

#include <glad/glad.h>

namespace Marx
{
	void OpenGLRendererAPI::init()
	{
		enableBlending(true);

		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::setClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLRendererAPI::clear()
	{
		OpenGLGraphicsContext::get()->clear(nullptr);
	}

	void OpenGLRendererAPI::drawIndexed(const Reference<VertexArray>& vertexArray)
	{
		glDrawElements(
			GL_TRIANGLES,
			vertexArray->getIndexBuffer()->getCount(),
			GL_UNSIGNED_INT,
			nullptr
		);
	}

	void OpenGLRendererAPI::enableDepthTest(bool enabled)
	{
		OpenGLGraphicsContext::get()->enableDepthTest(enabled);
	}

	void OpenGLRendererAPI::enableBlending(bool enabled)
	{
		OpenGLGraphicsContext::get()->enableBlending(enabled);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		OpenGLGraphicsContext::get()->setViewport(x, y, width, height);
	}
}