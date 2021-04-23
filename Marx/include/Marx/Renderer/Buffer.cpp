#include "mxpch.h"
#include "Buffer.h"

#include "Renderer.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Marx/Platform/D3D11/D3D11Buffer.h"
#endif

namespace Marx
{
	void BufferLayout::calculateElementData()
	{
		{
			m_stride = 0;
			uint32_t offset = 0;
			for (auto& element : m_elements)
			{
				element.offset = offset;
				offset += element.size;
				m_stride += element.size;
			}
		}
	}

	Reference<VertexBuffer> VertexBuffer::create(const void* vertices, uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11VertexBuffer>(vertices, size);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	///////////////////////////////////////
	// ---------- IndexBuffer ---------- //
	///////////////////////////////////////

	Reference<IndexBuffer> IndexBuffer::create(const uint32_t* indices, uint32_t count, PrimitiveType primType)
    {
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<DX11IndexBuffer>(indices, count, primType);
#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
    }

	//////////////////////////////////////////
	// ---------- ConstantBuffer ---------- //
	//////////////////////////////////////////

	Reference<ConstantBuffer> VSConstantBuffer::create(const void* data, uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11VSConstantBuffer>(data, size);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Reference<ConstantBuffer> PSConstantBuffer::create(const void* data, uint32_t size)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11PSConstantBuffer>(data, size);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
