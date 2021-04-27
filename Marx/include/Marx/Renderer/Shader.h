#pragma once

namespace Marx
{
	enum class ShaderType : uint32_t
	{
		None,
		Vertex,
		Pixel
	};

	enum class ShaderDataType
	{
		None = 0,
		Float1, Float2, Float3, Float4,
		Mat3, Mat4,
		Int1, Int2, Int3, Int4,
		Bool1
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:   return 0;
		case ShaderDataType::Float1: return 4 * 1;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;
		case ShaderDataType::Mat3:   return 4 * 3 * 3;
		case ShaderDataType::Mat4:   return 4 * 4 * 4;
		case ShaderDataType::Int1:   return 4 * 1;
		case ShaderDataType::Int2:   return 4 * 2;
		case ShaderDataType::Int3:   return 4 * 3;
		case ShaderDataType::Int4:   return 4 * 4;
		case ShaderDataType::Bool1:  return 1 * 1;
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

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

		virtual void updateUniform(const std::string& name, const void* data, ShaderDataType sdt) = 0;
	public:
		static std::string readFile(const std::string& filepath);
		static std::string extractNameFromFilepath(const std::string& filepath);
		static std::unordered_map<ShaderType, std::string> preprocess(std::string shaderSrc);
	};

	class ShaderLib
	{
	public:
		void add(const Reference<Shader>& shader);
		Reference<Shader> load(std::string filepath, bool autoDetectType = false);
		Reference<Shader> load(std::string filepath, const std::string& name, bool autoDetectType = false);
	public:
		Reference<Shader> get(const std::string& name);
		bool exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Reference<Shader>> m_shaders;
	};
}