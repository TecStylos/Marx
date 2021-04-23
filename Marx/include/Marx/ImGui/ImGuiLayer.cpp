#include "mxpch.h"
#include "ImGuiLayer.h"

#include "Marx/Application.h"
#include "Marx/Input/KeyCodes.h"

#if defined MX_PLATFORM_WINDOWS
	#include "Marx/Platform/Win32/Win32Window.h"
	#include "Marx/Platform/D3D11/D3D11GraphicsContext.h"

	#include "backends/imgui_impl_dx11.h"
	#include "backends/imgui_impl_win32.h"
#elif defined MX_PLATFORM_UNIX
	#include "backends/imgui_impl_glfw.h"
	#include "backends/imgui_impl_opengl3.h"

	#include <GLFW/glfw3.h>
	#include <glad/glad.h>
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
		#if defined MX_PLATFORM_WINDOWS
			ImGui_ImplDX11_Init(D3D11GraphicsContext::D3D11Manager::getDevice(), D3D11GraphicsContext::D3D11Manager::getContext());
			ImGui_ImplWin32_Init(nativeWindow);
		#elif defined MX_PLATFORM_UNIX
			ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)nativeWindow, true);
			ImGui_ImplOpenGL3_Init("#version 410");
		#endif
	}

	void ImGuiLayer::onDetach()
	{
		#if defined MX_PLATFORM_WINDOWS
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
		#elif defined MX_PLATFORM_UNIX
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		#endif

		ImGui::DestroyContext();
	}

	void ImGuiLayer::onImGuiRender()
	{
		static bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::begin()
	{
		#if defined MX_PLATFORM_WINDOWS
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
		#elif defined MX_PLATFORM_UNIX
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		#endif

		ImGui::NewFrame();
	}

	void ImGuiLayer::end()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application* app = Application::get();
		io.DisplaySize = ImVec2((float)app->getWindow()->getWidth(), (float)app->getWindow()->getHeight());

		ImGui::Render();

		#if defined MX_PLATFORM_WINDOWS
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		#elif defined MX_PLATFORM_UNIX
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		#endif
		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}