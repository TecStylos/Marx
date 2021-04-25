#include "mxpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#ifdef MX_ENABLE_D3D11
	#include "Marx/Platform/D3D11/D3D11VertexArray.h"
#endif
#ifdef MX_ENABLE_OPENGL
#include "Marx/Platform/OpenGL/OpenGLVertexArray.h"
#endif

namespace Marx
{
	Reference<VertexArray> VertexArray::create()
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
			#ifdef MX_ENABLE_D3D11
			return std::make_shared<D3D11VertexArray>();
			#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
			#endif
		case RendererAPI::API::OpenGL:
			#ifdef MX_ENABLE_OPENGL
			return std::make_shared<OpenGLVertexArray>();
			#else
			MX_CORE_ASSERT(false, "RendererAPI::OpenGL is not supported!");
			return nullptr;
			#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Reference<VertexArray> VertexArray::create(const Reference<VertexBuffer>& vertexBuffer, const Reference<IndexBuffer>& indexBuffer)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
			#ifdef MX_ENABLE_D3D11
			return std::make_shared<D3D11VertexArray>(vertexBuffer, indexBuffer);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
			#endif
		case RendererAPI::API::OpenGL:
			#ifdef MX_ENABLE_OPENGL
			return std::make_shared<OpenGLVertexArray>(vertexBuffer, indexBuffer);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::OpenGL is not supported!");
			return nullptr;
			#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}