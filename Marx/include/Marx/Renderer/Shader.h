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
		static Reference<Shader> create(const std::string& filepath, const std::string& name);
		static Reference<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc);
	public:
		virtual ~Shader() {};
	public:
		virtual void bind() const = 0;

		virtual const std::string& getName() const = 0;
	public:
		static std::string readFile(const std::string& filepath);
		static std::string extractNameFromFilepath(const std::string& filepath);
	};

	class ShaderLib
	{
	public:
		void add(const Reference<Shader>& shader);
		Reference<Shader> load(const std::string& filepath);
		Reference<Shader> load(const std::string& filepath, const std::string& name);
	public:
		Reference<Shader> get(const std::string& name);
		bool exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Reference<Shader>> m_shaders;
	};
}