cmake_minimum_required(VERSION 3.8)

project ("Sandbox")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(
	Sandbox
	"src/SandboxApp.cpp"
	"src/ControllerVisualizer.cpp"
)

target_include_directories(
	Sandbox PUBLIC 
	${MARX_EXT_INCLUDE_DIRECTORIES}
)

target_link_libraries(
	Sandbox PUBLIC
	Marx
)

set(
	SANDBOX_COMPILE_OPTIONS
	${SANDBOX_COMPILE_OPTIONS}
	${MARX_EXT_COMPILE_OPTIONS}
)

target_compile_options(
	Sandbox PUBLIC 
	${SANDBOX_COMPILE_OPTIONS}
)