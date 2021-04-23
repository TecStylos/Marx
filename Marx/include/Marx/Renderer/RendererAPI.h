#pragma once

#include "Buffer.h"
#include "VertexArray.h"

namespace Marx
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			D3D11 = 1,
			OpenGL = 2,
		};
	public:
		virtual void init() = 0;
		virtual void clear() = 0;
		virtual void setClearColor(float r, float g, float b, float a) = 0;
		virtual void drawIndexed(const Reference<VertexArray>& vertexArray) = 0;
		virtual void enableDepthTest(bool enabled) = 0;
		virtual void enableBlending(bool enabled) = 0;
	public:
		inline static API getAPI() { return s_API; }
	private:
		static API s_API;
	};
}