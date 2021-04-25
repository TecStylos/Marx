#pragma once

#include <string>

#include "Marx/Renderer/Shader.h"

namespace Marx
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath, const std::string& name);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc);
		~OpenGLShader();
	public:
		virtual void bind() const override;
		virtual const std::string& getName() const override { return m_name; }
	private:
		void compile(const std::unordered_map<ShaderType, std::string>& shaderSources);
	private:
		std::string m_name;
		uint32_t m_shader;
	};
}