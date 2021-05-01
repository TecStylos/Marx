#include "mxpch.h"

#include "D3D11Shader.h"

#include "Marx/Platform/D3D11/D3D11InfoManager.h"
#include "Marx/Platform/D3D11/D3D11InfoException.h"
#include "Marx/Platform/D3D11/D3D11ExceptionMacros.h"

#include "Marx/Platform/D3D11/D3D11GraphicsContext.h"

#include "D3D11Conversions.h"

namespace Marx
{
	D3D11Shader::D3D11Shader(const std::string& filepath, const std::string& name)
		: m_name(name)
	{
		auto shaderSrc = readFile(filepath);
		auto sources = preprocess(shaderSrc);
		detectConstBuffers(sources);
		compile(sources);
	}

	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc)
		: m_name(name)
	{
		MX_DEBUG_HR_DECL;

		ShaderSources sources;
		sources[ShaderType::Vertex] = vertexSrc;
		sources[ShaderType::Pixel] = pixelSrc;
		detectConstBuffers(sources);
		compile(sources);
	}

	D3D11Shader::~D3D11Shader()
	{}

	void D3D11Shader::bind() const
	{
		D3D11GraphicsContext::D3D11Manager::getContext()->VSSetShader(m_pVertexShader.Get(), 0, 0);
		D3D11GraphicsContext::D3D11Manager::getContext()->PSSetShader(m_pPixelShader.Get(), 0, 0);

		for (auto& elem : m_constBuffers)
			elem->bind();
	}

	void D3D11Shader::updateUniform(const std::string& name, const void* data, ShaderDataType sdt)
	{
		auto elem = m_uniforms.find(name);
		MX_CORE_ASSERT(elem != m_uniforms.end(), "Unknown constant buffer name!");
		m_constBuffers[elem->second.bufferIndex]->updatePartialStaged(data, elem->second.offset, elem->second.size);
	}

	const char* D3D11Shader::getTargetStringFromType(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Vertex:
			return "vs_5_0";
		case ShaderType::Pixel:
			return "ps_5_0";
		}

		MX_CORE_ASSERT(false, "Unknown ShaderType!");
		return "";
	}

	

	HRESULT D3D11Shader::compileSrc(const std::string& src, const char* pTarget, ID3DBlob** ppBytecode)
	{
		ComPtr<ID3DBlob> pErrBlob = nullptr;
		UINT flags = 0;
	#ifdef MX_DEBUG
		flags |= D3DCOMPILE_DEBUG;
	#endif
		HRESULT hr = D3DCompile(
			src.c_str(), src.size(), NULL,
			NULL, NULL, "main", pTarget,
			flags, 0, ppBytecode, pErrBlob.GetAddressOf()
		);
		if (pErrBlob)
		{
			std::string errString((char*)pErrBlob->GetBufferPointer(), pErrBlob->GetBufferSize());
			MX_CORE_ERROR("{0}", errString);
		}
		return hr;
	}

	void D3D11Shader::compile(const ShaderSources& shaderSources)
	{
		MX_DEBUG_HR_DECL;

		for (auto& source : shaderSources)
		{
			ComPtr<ID3DBlob> blob;
			MX_VERIFY_THROW_HR_INFO(compileSrc(source.second, getTargetStringFromType(source.first), blob.GetAddressOf()));

			switch (source.first)
			{
			case ShaderType::Vertex:
				MX_VERIFY_THROW_HR_INFO(D3D11GraphicsContext::D3D11Manager::getDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
				break;
			case ShaderType::Pixel:
				MX_VERIFY_THROW_HR_INFO(D3D11GraphicsContext::D3D11Manager::getDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
				break;
			default:
				MX_CORE_ASSERT(false, "Unknown shader type");
			}
		}
	}

	D3D11Shader::InternalConstBuffElement D3D11Shader::extractElementFromLine(const std::string& line)
	{
		const char* buffToken = "@cbuff ";
		size_t buffTokenLength = strlen(buffToken);

		size_t slotBegin = buffTokenLength;
		size_t slotEnd = line.find_first_of(" ", slotBegin);
		size_t typeBegin = slotEnd + 1;
		size_t typeEnd = line.find_first_of(" ", typeBegin);
		size_t nameBegin = typeEnd + 1;
		size_t nameEnd = line.find_first_of(";", nameBegin);
		MX_CORE_ASSERT(
			slotEnd != std::string::npos &&
			typeEnd != std::string::npos &&
			nameEnd != std::string::npos,
			"Syntax error!"
		);

		InternalConstBuffElement elem;
		elem.slot = std::stoi(line.substr(slotBegin, slotEnd - slotBegin));
		elem.type = hlslStringToShaderDataType(line.substr(typeBegin, typeEnd - typeBegin));
		elem.name = line.substr(nameBegin, nameEnd - nameBegin);

		return elem;
	}

	void D3D11Shader::detectConstBuffers(ShaderSources& shaderSources)
	{
		const char* buffToken = "@cbuff ";

		uint32_t buffIndex = 0;

		for (auto& shader : shaderSources)
		{
			std::unordered_map<uint32_t, std::vector<InternalConstBuffElement>> buffers;

			ShaderType type = shader.first;
			std::istringstream stream(shader.second);

			std::string outStr;

			std::string line;
			while (std::getline(stream, line))
			{
				if (line.find(buffToken) == 0)
				{
					auto elem = extractElementFromLine(line);
					buffers[elem.slot].push_back(elem);
				}
				else
				{
					outStr.append(line);
					outStr.push_back('\n');
				}
			}

			std::string cbuffStr;

			for (auto& buff : buffers)
			{
				cbuffStr.append(
					"cbuffer _CONSTANT_BUFFER_" + std::to_string(buff.first) +
					" : register(b" + std::to_string(buff.first) + ")\n{\n"
				);

				for (auto& elem : buff.second)
				{
					cbuffStr.append(
						"\t" + shaderDataTypeToHLSL_String(elem.type) + " " + elem.name + ";\n"
					);
				}

				cbuffStr.append("}\n");
			}

			for (auto& buff : buffers)
			{

				uint32_t offset = 0;

				for (auto& elem : buff.second)
				{
					ConstBufferElement cbelem;
					cbelem.bufferIndex = buffIndex;
					cbelem.offset = offset;
					cbelem.size = ShaderDataTypeSize(elem.type);
					m_uniforms[elem.name] = cbelem;

					offset += cbelem.size;
				}

				m_constBuffers.push_back(std::make_shared<D3D11ConstantBuffer>(type, offset, buff.first));

				++buffIndex;
			}

			shader.second = cbuffStr + outStr;
		}
	}
}