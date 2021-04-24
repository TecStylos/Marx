#pragma once

#include "RendererAPI.h"

namespace Marx
{
	class RenderCommand
	{
	public:
		static void init();
		inline static void setClearColor(float r, float g, float b, float a) { s_rendererAPI->setClearColor(r, g, b, a); }
		inline static void clear() { s_rendererAPI->clear(); }
		inline static void drawIndexed(const Reference<VertexArray>& vertexArray) { s_rendererAPI->drawIndexed(vertexArray); }
		inline static void enableDepthTest(bool enabled) { s_rendererAPI->enableDepthTest(enabled); }
		inline static void enableBlending(bool enabled) { s_rendererAPI->enableBlending(enabled); }
	private:
		static Scope<RendererAPI> s_rendererAPI;
	};
}