#include "mxpch.h"
#include "Texture.h"

#include "Renderer.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Marx/Platform/D3D11/D3D11Texture.h"
#endif

namespace Marx
{
	Reference<Texture2D> Texture2D::create(const std::string& path)
	{
		try
		{
			switch (Renderer::getAPI())
			{
			case RendererAPI::API::None:
				MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
				return nullptr;
			case RendererAPI::API::D3D11:
			#ifdef MX_PLATFORM_WINDOWS
				return std::make_shared<D3D11Texture2D>(path);
			#else
				MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
				return nullptr;
			#endif
			}
		}
		catch (TextureLoadException& e)
		{
			MX_CORE_ERROR("TextureLoadException: {0}", e.what());
			return nullptr;
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}