#include "mxpch.h"
#include "Shader.h"

#include "Renderer.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Marx/Platform/D3D11/D3D11Shader.h"
#endif

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
		return create(filepath, extractNameFromFilepath(filepath));
	}

	Reference<Shader> Shader::create(const std::string& filepath, const std::string& name)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11Shader>(filepath, name);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Reference<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::None is not supported!");
			return nullptr;
		case RendererAPI::API::D3D11:
		#ifdef MX_PLATFORM_WINDOWS
			return std::make_shared<D3D11Shader>(name, vertexSrc, pixelSrc);
		#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
		#endif
		}

		MX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::string Shader::readFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			MX_CORE_ERROR("Cannot open file '{0}'", filepath);
			return std::string();
		}
		std::string result;
		result.resize(file.tellg());
		file.seekg(0);
		file.read(&result[0], result.size());
		file.close();
		return result;
	}

	std::string Shader::extractNameFromFilepath(const std::string& filepath)
	{
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = (lastDot == std::string::npos) ? filepath.size() - lastSlash : lastDot - lastSlash;
		return filepath.substr(lastSlash, lastDot - lastSlash);
	}

	void ShaderLib::add(const Reference<Shader>& shader)
	{
		auto& name = shader->getName();
		MX_CORE_ASSERT(!exists(name), "Shader with same name already exists");
		m_shaders[name] = shader;
	}

	Reference<Shader> Marx::ShaderLib::load(const std::string& filepath)
	{
		return load(filepath, Shader::extractNameFromFilepath(filepath));
	}

	Reference<Shader> ShaderLib::load(const std::string& filepath, const std::string& name)
	{
		auto shader = Shader::create(filepath, name);
		add(shader);
		return shader;
	}

	Reference<Shader> ShaderLib::get(const std::string& name)
	{
		MX_CORE_ASSERT(exists(name), "Shader not found!");
		return m_shaders[name];
	}

	bool ShaderLib::exists(const std::string& name) const
	{
		return m_shaders.find(name) != m_shaders.end();
	}
}