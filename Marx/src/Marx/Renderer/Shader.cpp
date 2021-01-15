#include "mxpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Marx/Platform/D3D11/D3D11Shader.h"

namespace Marx
{
	ShaderType shaderTypeFromString(const std::string& str)
	{
		if (str == "vertex") return ShaderType::Vertex;
		if (str == "pixel") return ShaderType::Pixel;
		
		MX_CORE_ASSERT(false, "Invalid shader type");
		return (ShaderType)0;
	}

	Reference<Shader> Shader::create(const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11Shader>(filepath);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Reference<Shader> Shader::create(const std::string& vertexSrc, const std::string& pixelSrc)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11Shader>(vertexSrc, pixelSrc);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::string Shader::readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			MX_CORE_ERROR("Cannot open file '{0}'", filename);
			return std::string();
		}
		std::string result;
		result.resize(file.tellg());
		file.seekg(0);
		file.read(&result[0], result.size());
		file.close();
		return result;
	}
}