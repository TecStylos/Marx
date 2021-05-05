#include "mxpch.h"
#include "Shader.h"

#include "Renderer.h"

#ifdef MX_ENABLE_D3D11
#include "Marx/Platform/D3D11/D3D11Shader.h"
#endif
#ifdef MX_ENABLE_OPENGL
#include "Marx/Platform/OpenGL/OpenGLShader.h"
#endif

namespace Marx
{
	static const char* getShaderFileExtension()
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MX_CORE_ASSERT(false, "RendererAPI::API::None is not supported!");
			return ".UNKNOWN";
		case RendererAPI::API::OpenGL:
			return ".glsl";
		case RendererAPI::API::D3D11:
			return ".hlsl";
		}
		MX_CORE_ASSERT(false, "Unknown RendererAPI::API!");
		return ".UNKNOWN";
	}

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
			#ifdef MX_ENABLE_D3D11
			return std::make_shared<D3D11Shader>(filepath, name);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
			#endif
		case RendererAPI::API::OpenGL:
			#ifdef MX_ENABLE_OPENGL
			return std::make_shared<OpenGLShader>(filepath, name);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::OpenGL is not supported!");
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
			#ifdef MX_ENABLE_D3D11
			return std::make_shared<D3D11Shader>(name, vertexSrc, pixelSrc);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::D3D11 is not supported!");
			return nullptr;
			#endif
		case RendererAPI::API::OpenGL:
			#ifdef MX_ENABLE_OPENGL
			return std::make_shared<OpenGLShader>(name, vertexSrc, pixelSrc);
			#else
			MX_CORE_ASSERT(false, "RendererAPI::OpenGL is not supported!");
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

	std::unordered_map<ShaderType, std::string> Shader::preprocess(std::string shaderSrc)
	{
		std::unordered_map<ShaderType, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = shaderSrc.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = shaderSrc.find_first_of("\r\n", pos);
			MX_CORE_ASSERT(eol != std::string::npos, "Syntax error!");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = shaderSrc.substr(begin, eol - begin);
			MX_CORE_ASSERT(type == "vertex" || type == "pixel", "Invalid shader type!");

			size_t nextLinePos = shaderSrc.find_first_not_of("\r\n", eol);
			pos = shaderSrc.find(typeToken, nextLinePos);
			shaderSources[shaderTypeFromString(type)] = shaderSrc.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderSrc.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void ShaderLib::add(const Reference<Shader>& shader)
	{
		auto& name = shader->getName();
		MX_CORE_ASSERT(!exists(name), "Shader with same name already exists");
		m_shaders[name] = shader;
	}

	Reference<Shader> Marx::ShaderLib::load(std::string filepath, bool autoDetectType)
	{
		if (autoDetectType)
			filepath += getShaderFileExtension();
		return load(filepath, Shader::extractNameFromFilepath(filepath), false);
	}

	Reference<Shader> ShaderLib::load(std::string filepath, const std::string& name, bool autoDetectType)
	{
		if (autoDetectType)
			filepath += getShaderFileExtension();
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