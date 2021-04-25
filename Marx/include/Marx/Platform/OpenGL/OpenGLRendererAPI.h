#pragma once

#include "Marx/Renderer/RendererAPI.h"

namespace Marx
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;
		virtual void setClearColor(float r, float g, float b, float a) override;
		virtual void clear() override;
		virtual void drawIndexed(const Reference<VertexArray>& vertexArray) override;
		virtual void enableDepthTest(bool enabled) override;
		virtual void enableBlending(bool enabled) override;
	};
}