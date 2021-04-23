#include "mxpch.h"
#include "RendererAPI.h"

namespace Marx
{
	#if defined MX_PLATFORM_WINDOWS
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::D3D11;
	#elif defined MX_PLATFORM_UNIX
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
	#endif
}