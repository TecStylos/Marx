cmake_minimum_required(VERSION 3.8)

project ("Sandbox")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(
	Sandbox
	"src/SandboxApp.cpp"
)

target_include_directories(
	Sandbox PUBLIC 
	"${CMAKE_SOURCE_DIR}/Marx/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor/spdlog/include"
	"${CMAKE_SOURCE_DIR}/Marx/vendor"
)

target_link_libraries(
	Sandbox PUBLIC
	Marx
)

set(
	SANDBOX_COMPILE_OPTIONS
)

if (WIN32)
	set(
		SANDBOX_COMPILE_OPTIONS
		${SANDBOX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_WINDOWS"
	)
elseif(UNIX AND NOT APPLE)
	set(
		SANDBOX_COMPILE_OPTIONS
		${SANDBOX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_UNIX"
		"-DGLFW_INCLUDE_VULKAN"
	)
elseif(APPLE)
	set(
		SANDBOX_COMPILE_OPTIONS
		${SANDBOX_COMPILE_OPTIONS}
		"-DMX_PLATFORM_MACOS"
		"-DGLFW_INCLUDE_VULKAN"
	)
endif()

target_compile_options(
	Sandbox PUBLIC 
	${SANDBOX_COMPILE_OPTIONS}
)