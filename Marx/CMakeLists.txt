cmake_minimum_required (VERSION 3.8)

project ("Marx")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(
	Marx STATIC
	"include/Marx/Application.cpp"
	"include/Marx/Layer.cpp"
	"include/Marx/LayerStack.cpp"
	"include/Marx/Core/Log.cpp"
	"include/Marx/ImGui/ImGuiLayer.cpp"
	"include/Marx/Input/ControllerManager.cpp"
	"include/Marx/Platform/D3D11/D3D11Buffer.cpp"
	"include/Marx/Platform/D3D11/D3D11GraphicsContext.cpp"
	"include/Marx/Platform/D3D11/D3D11InfoManager.cpp"
	"include/Marx/Platform/D3D11/D3D11RendererAPI.cpp"
	"include/Marx/Platform/D3D11/D3D11Shader.cpp"
	"include/Marx/Platform/D3D11/D3D11Texture.cpp"
	"include/Marx/Platform/D3D11/D3D11VertexArray.cpp"
	"include/Marx/Platform/Win32/Win32Input.cpp"
	"include/Marx/Platform/Win32/Win32Window.cpp"
	"include/Marx/Platform/XInput/XController.cpp"
	"include/Marx/Renderer/Buffer.cpp"
	"include/Marx/Renderer/Camera.cpp"
	"include/Marx/Renderer/RenderCommand.cpp"
	"include/Marx/Renderer/Renderer.cpp"
	"include/Marx/Renderer/RendererAPI.cpp"
	"include/Marx/Renderer/Shader.cpp"
	"include/Marx/Renderer/Texture.cpp"
	"include/Marx/Renderer/VertexArray.cpp"

	"vendor/stb_image/stb_image.cpp"
	"$<$<BOOL:WIN32>:vendor/imgui/backends/imgui_impl_win32.cpp>"
	"$<$<BOOL:WIN32>:vendor/imgui/backends/imgui_impl_dx11.cpp>"
)

if (WIN32)
	target_compile_options(
		Marx PUBLIC
		"-DMX_PLATFORM_WINDOWS"
		"$<$<CONFIG:DEBUG>:-DMX_ENABLE_ASSERTS>"
	)
endif()

target_include_directories(
	Marx PUBLIC 
	"include"

	"vendor/imgui"
	"vendor/spdlog/include"
	"vendor/stb_image"
)

include("FindDirectX11.cmake")

target_link_libraries(
	Marx PUBLIC
	imgui
	${DirectX11_LIBRARY}
)

target_precompile_headers (
	Marx PUBLIC 
	"include/mxpch.h"
)