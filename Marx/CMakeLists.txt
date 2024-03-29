cmake_minimum_required (VERSION 3.8)

project ("Marx")

add_subdirectory("vendor/imgui")
add_subdirectory("vendor/glad")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("vendor/glfw")

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
	"include/Marx/Input/Controller.cpp"
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

	"include/Marx/Platform/GLFW/CrossWindow.cpp"
	"include/Marx/Platform/GLFW/CrossInput.cpp"
	"include/Marx/Platform/GLFW/CrossController.cpp"
	"include/Marx/Platform/OpenGL/OpenGLGraphicsContext.cpp"
	"include/Marx/Platform/OpenGL/OpenGLRendererAPI.cpp"
	"include/Marx/Platform/OpenGL/OpenGLBuffer.cpp"
	"include/Marx/Platform/OpenGL/OpenGLVertexArray.cpp"
	"include/Marx/Platform/OpenGL/OpenGLShader.cpp"
	"include/Marx/Platform/OpenGL/OpenGLTexture.cpp"

	"vendor/imgui/backends/imgui_impl_glfw.cpp"
	"vendor/imgui/backends/imgui_impl_opengl3.cpp"
	"include/Marx/Input/KeyCodeConverter.cpp"
)

set(
	MARX_COMPILE_OPTIONS
)

set(
	MARX_LINK_LIBRARIES
	GLAD imgui
)

set(
	MARX_INCLUDE_DIRECTORIES
	"include"
	"vendor"
	"vendor/imgui"
	"vendor/spdlog/include"
	"vendor/stb_image"
	"vendor/glm"
	"vendor/glfw/include"
	"vendor/glad/include"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_ENABLE_ASSERTS"
		"-DMX_DEBUG"
		"-DMX_ENABLE_PROFILING"
	)
endif()

if (WIN32)
	set(
		MARX_LIB_FILES
		${MARX_LIB_FILES}
		"include/Marx/Platform/D3D11/D3D11Buffer.cpp"
		"include/Marx/Platform/D3D11/D3D11GraphicsContext.cpp"
		"include/Marx/Platform/D3D11/D3D11InfoManager.cpp"
		"include/Marx/Platform/D3D11/D3D11RendererAPI.cpp"
		"include/Marx/Platform/D3D11/D3D11Shader.cpp"
		"include/Marx/Platform/D3D11/D3D11Texture.cpp"
		"include/Marx/Platform/D3D11/D3D11VertexArray.cpp"
		"include/Marx/Platform/D3D11/D3D11ConstantBuffers.cpp"
		"include/Marx/Platform/Win32/Win32Input.cpp"
		"include/Marx/Platform/Win32/Win32Window.cpp"
		"include/Marx/Platform/XInput/XController.cpp"

		"vendor/imgui/backends/imgui_impl_win32.cpp"
		"vendor/imgui/backends/imgui_impl_dx11.cpp"
	)
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_WINDOWS"
	)
	set(
		MARX_LINK_LIBRARIES
		${MARX_LINK_LIBRARIES}
		Xinput.lib d3d11.lib dxgi.lib dxguid.lib glfw
	)
elseif(UNIX AND NOT APPLE)
	set(
		MARX_LIB_FILES
		${MARX_LIB_FILES}
		"include/Marx/Platform/Unix/UnixController.cpp"
	)
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_UNIX"
	)
	set(
		MARX_LINK_LIBRARIES
		${MARX_LINK_LIBRARIES}
		dl glfw GL GLEW
	)
elseif(APPLE)
	set(
		MARX_COMPILE_OPTIONS
		${MARX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_MACOS"
	)

	set(
		MARX_LINK_LIBRARIES
		${MARX_LINK_LIBRARIES}
		dl glfw
	)
endif()

add_library(
	Marx STATIC
	${MARX_LIB_FILES}
)

target_include_directories(
	Marx PUBLIC 
	${MARX_INCLUDE_DIRECTORIES}
)

target_compile_options(
	Marx PUBLIC 
	${MARX_COMPILE_OPTIONS}
)

target_compile_definitions(
	Marx PUBLIC
	${MARX_COMPILE_DEFS}
)

target_link_libraries(
	Marx PUBLIC
	${MARX_LINK_LIBRARIES}
)

target_precompile_headers (
	Marx PUBLIC 
	"include/mxpch.h"
)

set(
	MARX_EXT_COMPILE_OPTIONS
	${MARX_COMPILE_OPTIONS}
)

set(
	MARX_EXT_INCLUDE_DIRECTORIES
	"${CMAKE_SOURCE_DIR}/Marx/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor/spdlog/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor"
)