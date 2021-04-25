#include "mxpch.h"
#include "RenderCommand.h"

#ifdef MX_ENABLE_D3D11
	#include "Marx/Platform/D3D11/D3D11RendererAPI.h"
#endif
#ifdef MX_ENABLE_OPENGL
#include "Marx/Platform/OpenGL/OpenGLRendererAPI.h""
#endif

namespace Marx
{
	Scope<RendererAPI> RenderCommand::s_rendererAPI = nullptr;

	void RenderCommand::init()
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "A renderer API must be selected before calling RenderCommand::init()!");
			break;
		case RendererAPI::API::OpenGL:
			#ifdef MX_ENABLE_OPENGL
			s_rendererAPI = std::make_shared<OpenGLRendererAPI>();
			#else
			MX_CORE_ASSERT(false, "The renderer API 'OpenGL' is not supported by this configuration!");
			#endif
			break;
		case RendererAPI::API::D3D11:
			#ifdef MX_ENABLE_D3D11
			s_rendererAPI = std::make_unique<D3D11RendererAPI>();
			#else
			MX_CORE_ASSERT(false, "The renderer API 'D3D11' is not supported by this configuration!");
			#endif
			break;
		default:
			MX_CORE_ASSERT(false, "The selected renderer API is not supported!");
			break;
		}

		s_rendererAPI->init();
	}
}
