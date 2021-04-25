#include "mxpch.h"
#include "ImGuiLayer.h"

#include "Marx/Application.h"
#include "Marx/Input/KeyCodes.h"
#include "Marx/Renderer/RendererAPI.h"

#if defined MX_ENABLE_D3D11
	#include "Marx/Platform/Win32/Win32Window.h"
	#include "Marx/Platform/D3D11/D3D11GraphicsContext.h"

	#include "backends/imgui_impl_dx11.h"
	#include "backends/imgui_impl_win32.h"
#endif
#if defined MX_ENABLE_OPENGL
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
	#include <backends/imgui_impl_glfw.h>
	#include <backends/imgui_impl_opengl3.h>
#endif

namespace Marx
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer"), m_time(m_clock.now())
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::onAttach()
	{
		// Setup Dear ImGui context
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		void* nativeWindow = Application::get()->getWindow()->getNativeWindow();

		// Setup Platform/Renderer backends
		switch (Window::getAPI())
		{
		case Window::API::GLFW:
			ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)nativeWindow, true);
			break;
		case Window::API::Win32:
			ImGui_ImplWin32_Init(nativeWindow);
			break;
		}

		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::D3D11:
			ImGui_ImplDX11_Init(D3D11GraphicsContext::D3D11Manager::getDevice(), D3D11GraphicsContext::D3D11Manager::getContext());
			break;
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_Init("#version 410");
		}
	}

	void ImGuiLayer::onDetach()
	{
		switch (RendererAPI::getAPI())
		{
			#ifdef MX_ENABLE_D3D11
		case RendererAPI::API::D3D11:
			ImGui_ImplDX11_Shutdown();
			break;
			#endif
			#ifdef MX_ENABLE_OPENGL
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_Shutdown();
			break;
			#endif
		}

		switch (Window::getAPI())
		{
			#ifdef MX_ENABLE_GLFW
		case Window::API::GLFW:
			ImGui_ImplGlfw_Shutdown();
			break;
			#endif
			#ifdef MX_ENABLE_WIN32
		case Window::API::Win32:
			ImGui_ImplWin32_Shutdown();
			break;
			#endif
		}

		ImGui::DestroyContext();
	}

	void ImGuiLayer::onImGuiRender()
	{
		static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::begin()
	{
		switch (RendererAPI::getAPI())
		{
			#ifdef MX_ENABLE_D3D11
		case RendererAPI::API::D3D11:
			ImGui_ImplDX11_NewFrame();
			break;
			#endif
			#ifdef MX_ENABLE_OPENGL
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_NewFrame();
			break;
			#endif
		}

		switch (Window::getAPI())
		{
			#ifdef MX_ENABLE_GLFW
		case Window::API::GLFW:
			ImGui_ImplGlfw_NewFrame();
			break;
			#endif
			#ifdef MX_ENABLE_WIN32
		case Window::API::Win32:
			ImGui_ImplWin32_NewFrame();
			break;
			#endif
		}

		ImGui::NewFrame();
	}

	void ImGuiLayer::end()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application* app = Application::get();
		io.DisplaySize = ImVec2((float)app->getWindow()->getWidth(), (float)app->getWindow()->getHeight());

		ImGui::Render();

		switch (RendererAPI::getAPI())
		{
			#ifdef MX_ENABLE_D3D11
		case RendererAPI::API::D3D11:
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			break;
			#endif
			#ifdef MX_ENABLE_OPENGL
		case RendererAPI::API::OpenGL:
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			break;
			#endif
		}
		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}