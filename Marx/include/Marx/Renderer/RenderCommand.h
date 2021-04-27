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
		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { s_rendererAPI->setViewport(x, y, width, height); }
	private:
		static Scope<RendererAPI> s_rendererAPI;
	};
}