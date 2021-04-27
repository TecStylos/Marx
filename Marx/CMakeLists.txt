cmake_minimum_required (VERSION 3.8)

project ("Marx")

add_subdirectory("vendor/imgui")
add_subdirectory("vendor/glfw")
add_subdirectory("vendor/glad")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(
	MARX_LIB_FILES
	"include/Marx/Application.cpp"
	"include/Marx/Layer.cpp"
	"include/Marx/LayerStack.cpp"
	"include/Marx/Window.cpp"
	"include/Marx/WindowAPISelector.cpp"
	"include/Marx/Input.cpp"
	"include/Marx/Core/Log.cpp"
	"include/Marx/ImGui/ImGuiLayer.cpp"
	"include/Marx/Input/ControllerManager.cpp"
	"include/Marx/Renderer/Buffer.cpp"
	"include/Marx/Renderer/Camera.cpp"
	"include/Marx/Renderer/RenderCommand.cpp"
	"include/Marx/Renderer/Renderer.cpp"
	"include/Marx/Renderer/RendererAPI.cpp"
	"include/Marx/Renderer/RendererAPISelector.cpp"
	"include/Marx/Renderer/Shader.cpp"
	"include/Marx/Renderer/Texture.cpp"
	"include/Marx/Renderer/VertexArray.cpp"

	"vendor/stb_image/stb_image.cpp"

	"vendor/imgui/backends/imgui_impl_glfw.cpp"
	"vendor/imgui/backends/imgui_impl_opengl3.cpp"

	"include/Marx/Platform/GLFW/CrossWindow.cpp"
	"include/Marx/Platform/GLFW/CrossInput.cpp"
	"include/Marx/Platform/OpenGL/OpenGLGraphicsContext.cpp"
	"include/Marx/Platform/OpenGL/OpenGLRendererAPI.cpp"
	"include/Marx/Platform/OpenGL/OpenGLBuffer.cpp"
	"include/Marx/Platform/OpenGL/OpenGLVertexArray.cpp"
	"include/Marx/Platform/OpenGL/OpenGLShader.cpp"
	"include/Marx/Platform/OpenGL/OpenGLTexture.cpp"
)

add_library(
	Marx STATIC
	${MARX_LIB_FILES}

	"$<$<BOOL:WIN32>:"
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
	
		"vendor/imgui/backends/imgui_impl_win32.cpp"
		"vendor/imgui/backends/imgui_impl_dx11.cpp"
	">"
)

set(
	MARX_COMPILE_OPTIONS
	"$<$<CONFIG:DEBUG>:-DMX_ENABLE_ASSERTS>"
)

set(
	MARX_LINK_LIBRARIES
	imgui GLFW GLAD
)

set(
	MARX_INCLUDE_DIRECTORIES
	"include"

	"vendor/imgui"
	"vendor/spdlog/include"
	"vendor/stb_image"
	"vendor/glm"
	"vendor/glfw/include"
	"vendor/glad/include"
)

if (WIN32)
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_WINDOWS"
	)
	set(
		MARX_LINK_LIBRARIES
		${MARX_LINK_LIBRARIES}
		Xinput.lib d3d11.lib dxgi.lib dxguid.lib
	)
elseif(UNIX)
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_UNIX"
		"-Wl,--no-as-needed"
	)
endif()

target_include_directories(
	Marx PUBLIC 
	${MARX_INCLUDE_DIRECTORIES}
)

target_compile_options(
	Marx PUBLIC 
	${MARX_COMPILE_OPTIONS}
)

target_link_libraries(
	Marx PUBLIC
	${MARX_LINK_LIBRARIES}
)

target_precompile_headers (
	Marx PUBLIC 
	"include/mxpch.h"
)