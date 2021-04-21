#include "mxpch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Marx/Platform/D3D11/D3D11VertexArray.h"

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
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11VertexArray>();
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
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
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11VertexArray>(vertexBuffer, indexBuffer);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}