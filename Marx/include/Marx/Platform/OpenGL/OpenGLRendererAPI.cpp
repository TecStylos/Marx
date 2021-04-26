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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
}