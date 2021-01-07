#include "mxpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Marx/Platform/D3D11/D3D11Shader.h"

namespace Marx
{
	Shader* Shader::create(const std::string& vertexSrc, const std::string& pixelSrc)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return new D3D11Shader(vertexSrc, pixelSrc);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::string Shader::loadShaderSrcFromFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			MX_CORE_ERROR("Cannot open file '{0}'", filename);
			return std::string();
		}
		uint32_t fsize = (uint32_t)file.tellg();
		std::vector<char> buff(fsize);
		file.seekg(0);
		file.read(buff.data(), fsize);
		file.close();
		return std::string(buff.data(), fsize);
	}
}