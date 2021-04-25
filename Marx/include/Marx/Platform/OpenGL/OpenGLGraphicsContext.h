#pragma once

#include "Marx/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Marx
{
	class OpenGLGraphicsContext : public GraphicsContext
	{
	public:
		OpenGLGraphicsContext(GLFWwindow* wnd);
		~OpenGLGraphicsContext();
	public:
		virtual void init() override;
		virtual void shutdown() override;
		virtual void clear(float color[4]) override;
		virtual void swapBuffers() override;
		virtual void onResize(uint32_t width, uint32_t height) override;
		virtual void enableDepthTest(bool enabled) override;
		virtual void enableBlending(bool enabled) override;
		virtual void enableVSync(bool enabled) override;
	public:
		static GraphicsContext* get(uint32_t index = 0) { return s_contexts[index]; }
	private:
		GLFWwindow* m_wnd;
	private:
		static std::vector<OpenGLGraphicsContext*> s_contexts;
	};
}