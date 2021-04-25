#include "mxpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

namespace Marx
{
	static GLenum ShaderTypeToGLenum(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Vertex: return GL_VERTEX_SHADER;
		case ShaderType::Pixel: return GL_FRAGMENT_SHADER;
		}

		MX_CORE_ASSERT(false, "Unknown ShaderType!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath, const std::string& name)
		: m_name(name)
	{
		auto shaderSrc = readFile(filepath);
		auto sources = preprocess(shaderSrc);
		compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc)
		: m_name(name)
	{
		std::unordered_map<ShaderType, std::string> sources;
		sources[ShaderType::Vertex] = vertexSrc;
		sources[ShaderType::Pixel] = pixelSrc;
		compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_shader);
	}

	void OpenGLShader::compile(const std::unordered_map<ShaderType, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		MX_CORE_ASSERT(shaderSources.size() == 2, "Number of shaders must be 2!");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = ShaderTypeToGLenum(kv.first);
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				MX_CORE_ERROR("{0}", infoLog.data());
				MX_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_shader = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			MX_CORE_ERROR("{0}", infoLog.data());
			MX_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(m_shader);
	}
}