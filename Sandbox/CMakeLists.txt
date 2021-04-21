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

if (WIN32)
	target_compile_options(
		Sandbox PUBLIC 
		"-DMX_PLATFORM_WINDOWS"
	)
endif()