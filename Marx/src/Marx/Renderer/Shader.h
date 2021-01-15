#pragma once

namespace Marx
{
	enum class ShaderType : uint32_t
	{
		Vertex,
		Pixel,
	};

	ShaderType shaderTypeFromString(const std::string& str);

	class Shader
	{
	public:
		static Reference<Shader> create(const std::string& filepath);
		static Reference<Shader> create(const std::string& vertexSrc, const std::string& pixelSrc);
	public:
		virtual ~Shader() {};
	public:
		virtual void bind() const = 0;
	public:
		static std::string readFile(const std::string& filename);
	};
}