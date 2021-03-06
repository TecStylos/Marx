#pragma once

#include "Marx/Window.h"

#include "Marx/Renderer/GraphicsContext.h"

namespace Marx
{
	class MARX_API Win32Window : public Window
	{
	public:
		Win32Window(const WindowDesc& desc);
		virtual ~Win32Window();
	public:
		virtual void onUpdate() override;
		inline uint32_t getWidth() const override { return m_width; }
		inline uint32_t getHeight() const override { return m_height; }
		virtual void* getNativeWindow() const override { return m_hWnd; }
		virtual GraphicsContext& getGfxContext() const override { return *m_pContext; }
		virtual void enableImGuiFallthrough(bool enable) override { m_imGuiFallthroughEnabled = enable; }
		virtual void setTitle(const std::string& title) override { m_title = title; SetWindowTextA(m_hWnd, title.c_str()); }
		virtual uint32_t getWndCount() const override { return (uint32_t)Win32Manager::getWndCount(); }
	public:
		inline void setEventCallback(const EventCallbackFunc& callback) override { m_eventCallback = callback; }
	protected:
		virtual void init(const WindowDesc& desc) override;
		virtual void shutdown() override;
	protected:
		LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_minWidth = 200;
		uint32_t m_minHeight = 150;
		HWND m_hWnd;
		bool m_initialized = false;
		bool m_imGuiFallthroughEnabled = false;
		DISABLE_DLL_INTERFACE_WARN;
		std::string m_title;
		EventCallbackFunc m_eventCallback = [](Event&) {};
		REENABLE_DLL_INTERFACE_WARN;
		GraphicsContext* m_pContext;
	private:
		class Win32Manager
		{
		public:
			enum class Procedure
			{
				Setup,
				Default
			};
		public:
			static void init();
			static void shutdown();
			static HINSTANCE getInstance() { return GetModuleHandle(NULL); }
			static const char* getName() { return m_name; }
			static void registerWindow(HWND hWnd, Win32Window* pWnd);
			static void unregisterWindow(HWND hWnd);
			static void setProcedure(Procedure proc = Procedure::Default, HWND hWnd = nullptr);
			static size_t getWndCount() { return m_mapWindows.size(); }
			static Win32Window* getWnd(unsigned int index);
		public:
			static std::optional<int> handleMessages(HWND hWnd);
		private:
			static LRESULT CALLBACK wndProcSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static LRESULT CALLBACK wndProcForward(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		private:
			inline static WNDCLASS m_wc;
			inline static const char* m_name = "Marx WindowClass";
		private:
			inline static std::unordered_map<HWND, Win32Window*> m_mapWindows;
		};
	};
}