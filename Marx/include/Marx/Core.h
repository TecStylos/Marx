#pragma once

#include <memory>

#if defined MX_PLATFORM_WINDOWS
   #if MX_DYNAMIC_LINK
      #ifdef MX_BUILD_DLL
         #define MARX_API __declspec(dllexport)
      #else
         #define MARX_API __declspec(dllimport)
      #endif
   #else
      #define MARX_API
   #endif
   #define MX_DEBUG_BREAK() __debugbreak()

   #define MX_ENABLE_D3D11
   #define MX_ENABLE_OPENGL
   #define MX_ENABLE_WIN32
   #define MX_ENABLE_GLFW

   #define MX_OPENGL_HEADER <GL/gl.h>

   #define MX_FUNCTION_MACRO __FUNCSIG__
#elif defined MX_PLATFORM_UNIX
   #if MX_DYNAMIC_LINK
      #error Marx doesn't support dynamic linkage on unix platforms!
   #else
      #define MARX_API
   #endif
   #define MX_DEBUG_BREAK() __builtin_trap()

   #define MX_ENABLE_OPENGL
   #define MX_ENABLE_GLFW

   #define MX_OPENGL_HEADER <GL/gl.h>

   #define MX_FUNCTION_MACRO __PRETTY_FUNCTION__
#elif defined MX_PLATFORM_MACOS
   #if MX_DYNAMIX_LINK
      #error Marx doesn't support dynamic linkage in MacOS!
   #else
      #define MARX_API
   #endif
   #include <signal.h>
   #define MX_DEBUG_BREAK() raise(SIGTRAP)

   #define MX_ENABLE_OPENGL
   #define MX_ENABLE_GLFW

   #define MX_OPENGL_HEADER <OpenGL/GL.h>

   #define MX_FUNCTION_MACRO __PRETTY_FUNCTION__
#else
   #error Marx only supports Windows!
#endif

#if MX_DYNAMIC_LINK
  #define DISABLE_DLL_INTERFACE_WARN  __pragma(warning(push)) \
                                      __pragma(warning(disable:4251))
  #define REENABLE_DLL_INTERFACE_WARN __pragma(warning(pop))
#else
  #define DISABLE_DLL_INTERFACE_WARN
  #define REENABLE_DLL_INTERFACE_WARN
#endif

// Debug asserts
#ifdef MX_ENABLE_ASSERTS
#define MX_ASSERT(x, ...)      { if (!(x)) { MX_ERROR     ("Assertion failed! {0}", __VA_ARGS__); MX_DEBUG_BREAK(); } }
#define MX_CORE_ASSERT(x, ...) { if (!(x)) { MX_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); MX_DEBUG_BREAK(); } }
#else
#define MX_ASSERT(x, ...)
#define MX_CORE_ASSERT(x, ...)
#endif

// Bitwise Flag operations
#define FLAG(f) (1 << f)
#define SET_FLAG(x, f) (x = f)
#define ADD_FLAG(x, f) (x |= f)
#define REM_FLAG(x, f) (x &= ~f)
#define CLEAR_FLAG(x) (x = 0)
#define HAS_FLAG(x, f) (x & f)

// Binding method to class for event callbacks
#define MX_BIND_EVENT_METHOD_OBJ(obj, fn) std::bind(&fn, obj, std::placeholders::_1)
#define MX_BIND_EVENT_METHOD(fn) MX_BIND_EVENT_METHOD_OBJ(this, fn)

namespace Marx
{
    template<class T>
    using Scope = std::unique_ptr<T>;
    template<class T>
    using Reference = std::shared_ptr<T>;
}