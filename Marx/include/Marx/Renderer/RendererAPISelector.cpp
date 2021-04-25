#include "mxpch.h"
#include "RendererAPISelector.h"

#include "Marx/Renderer/RendererAPI.h"

namespace Marx
{
	void selectRendererAPIFromCmdLine(int argc, char* argv[])
	{
		#if defined MX_ENABLE_D3D11
		RendererAPI::setAPI(RendererAPI::API::D3D11);
		#elif defined MX_ENABLE_OPENGL
		RendererAPI::setAPI(RendererAPI::API::OpenGL);
		#endif

		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			if ("--opengl" == arg)
				RendererAPI::setAPI(Marx::RendererAPI::API::OpenGL);
			else if ("--d3d11" == arg)
				RendererAPI::setAPI(Marx::RendererAPI::API::D3D11);
		}
	}

	void printSupportedRendererAPIs()
	{
		std::cout << "Supported APIs: ";
		#ifdef MX_ENABLE_D3D11
		std::cout << "D3D11 ";
		#endif
		#ifdef MX_ENABLE_OPENGL
		std::cout << "OpenGL ";
		#endif
		std::cout << std::endl;
	}
}