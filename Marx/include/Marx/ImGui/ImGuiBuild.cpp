#include "mxpch.h"

#ifdef MX_ENABLE_WIN32
#include "backends/imgui_impl_win32.cpp"
#endif
#ifdef MX_ENABLE_D3D11
#include "backends/imgui_impl_dx11.cpp"
#endif

#ifdef MX_ENABLE_GLFW
#include "backends/imgui_impl_glfw.h"
#endif
#ifdef MX_ENABLE_OPENGL
#include "backends/imgui_impl_opengl3.h"
#endif