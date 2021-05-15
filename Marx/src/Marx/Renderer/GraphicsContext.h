#pragma once

namespace Marx
{
	class GraphicsContext
	{
	public:
		virtual void init() = 0;
		virtual void shutdown() = 0;
		virtual void clear(float color[4]) = 0;
		virtual void swapBuffers() = 0;
		virtual void onResize(unsigned int width, unsigned int height) = 0;
		virtual void enableDepthTest(bool enabled) = 0;
		virtual void enableBlending(bool enabled) = 0;
	};
}