#pragma once

#include "Marx/Window.h"
#include "Marx/Renderer/GraphicsContext.h"

#include <glfw/glfw3.h>

namespace Marx
{
	class CrossWindow : public Window
	{
	public:
		CrossWindow(const WindowDesc& desc);
		virtual ~CrossWindow();
	public:
		virtual void onUpdate() override;
		inline uint32_t getWidth() const override { return m_data.width; }
		inline uint32_t getHeight() const override { return m_data.height; }
		virtual void* getNativeWindow() const override { return m_wnd; }
		virtual GraphicsContext& getGfxContext() const override { return *m_pContext; }
		virtual void enableImGuiFallthrough(bool enable) override { m_data.imGuiFallthroughEnabled = enable; }
		virtual void setTitle(const std::string& title) override {};
		virtual uint32_t getWndCount() const override { return m_initialized ? 1 : 0; }
	public:
		inline void setEventCallback(const EventCallbackFunc& callback) override { m_data.eventCallback = callback; }
	protected:
		virtual void init(const WindowDesc& desc) override;
		virtual void shutdown() override;
	protected:
		struct WndData
		{
			uint32_t width;
			uint32_t height;
			bool imGuiFallthroughEnabled = false;
			std::string title;
			EventCallbackFunc eventCallback = [](Event&) {};
			Window* pWnd;
		} m_data;
		GLFWwindow* m_wnd;
		bool m_initialized = false;
		GraphicsContext* m_pContext;
	};
}