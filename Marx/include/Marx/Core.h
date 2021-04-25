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
#elif defined MX_PLATFORM_UNIX
   #if MX_DYNAMIC_LINK
      #error Marx doesn't support dynamice linkage on unix platforms!
   #else
      #define MARX_API
   #endif
   #define MX_DEBUG_BREAK() __builtin_trap()

   #define MX_ENABLE_OPENGL
   #define MX_ENABLE_GLFW
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
#define MX_BIND_EVENT_METHOD(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Marx
{
    template<class T>
    using Scope = std::unique_ptr<T>;
    template<class T>
    using Reference = std::shared_ptr<T>;
}