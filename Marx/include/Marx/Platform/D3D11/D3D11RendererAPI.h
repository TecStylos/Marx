#pragma once

#include "Marx/Renderer/RendererAPI.h"

namespace Marx
{
	class D3D11RendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;
		virtual void setClearColor(float r, float g, float b, float a) override;
		virtual void clear() override;
		virtual void drawIndexed(const Reference<VertexArray>& vertexArray) override;
		virtual void enableDepthTest(bool enabled) override;
		virtual void enableBlending(bool enabled) override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	private:
		float m_clearColor[4];
	};
}