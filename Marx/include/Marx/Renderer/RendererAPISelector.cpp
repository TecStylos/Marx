#include "RendererAPISelector.h"

#include "Marx/Renderer/RendererAPI.h"

namespace Marx
{
	void selectRendererAPIFromCmdLine(int argc, char* argv[])
	{
		#if defined MX_ENABLE_D3D11
		Marx::RendererAPI::setAPI(Marx::RendererAPI::API::D3D11);
		#elif defined MX_ENABLE_OPENGL
		Marx::RendererAPI::setAPI(Marx::RendererAPI::API::OpenGL);
		#endif

		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];

			if ("--supported-renderer-apis" == arg)
			{
				std::cout << "Supported APIs: ";
				#ifdef MX_ENABLE_D3D11
				std::cout << "D3D11 ";
				#endif
				#ifdef MX_ENABLE_OPENGL
				std::cout << "OpenGL ";
				#endif
				std::cout << std::endl;
				exit(0);
			}
			else if ("--opengl" == arg)
				Marx::RendererAPI::setAPI(Marx::RendererAPI::API::OpenGL);
			else if ("--d3d11" == arg)
				Marx::RendererAPI::setAPI(Marx::RendererAPI::API::D3D11);
		}
	}
}