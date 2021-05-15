#pragma once

#include "Marx/Core.h"
#include "Marx/Events/Event.h"

namespace Marx
{
	static bool s_windowClassInitialized = false;

	struct WindowDesc
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowDesc(const std::string& title = "Marx Engine", unsigned int width = 1280, unsigned int height = 720)
			: title(title), width(width), height(height)
		{}
	};

	class MARX_API Window
	{
	public:
		virtual ~Window() {}
	public:
		virtual void onUpdate() = 0;
		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual void* getNativeWindow() const = 0;
		virtual class GraphicsContext& getGfxContext() const = 0;
		virtual uint32_t getWndCount() const = 0;
		virtual void enableImGuiFallthrough(bool enable) = 0;
		virtual void setTitle(const std::string& title) = 0;
	public:
		virtual void shutdown() = 0;
	public:
		virtual void setEventCallback(const EventCallbackFunc& callback) = 0;
	public:
		static Window* create(const WindowDesc& desc = WindowDesc());
	};
}