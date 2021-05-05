#pragma once

#include "Buffer.h"
#include "VertexArray.h"

extern int main(int argc, char* argv[], char* env[]);

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
		virtual ~RendererAPI() = default;
	public:
		virtual void init() = 0;
		virtual void clear() = 0;
		virtual void setClearColor(float r, float g, float b, float a) = 0;
		virtual void drawIndexed(const Reference<VertexArray>& vertexArray) = 0;
		virtual void enableDepthTest(bool enabled) = 0;
		virtual void enableBlending(bool enabled) = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	public:
		inline static API getAPI() { return s_API; }
	private:
		inline static void setAPI(API api) { s_API = api; }
	private:
		static API s_API;
	private:
		friend void selectRendererAPIFromCmdLine(int argc, char* argv[]);
	};
}