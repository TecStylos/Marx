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

	void D3D11Shader::detectConstBuffers(ShaderSources& shaderSources)
	{
		const char* buffToken = "@cbuff ";
		size_t buffTokenLength = strlen(buffToken);

		uint32_t lastSlot = -1;
		uint32_t currIndex = -1;

		for (auto& shader : shaderSources)
		{
			bool cbuffOpen = false;
			uint32_t currOffset = 0;

			ShaderType type = shader.first;
			const std::string& source = shader.second;
			std::string constBuffStr;
			std::string outStr;

			size_t lineBegin = source.find_first_not_of("\r\n", 0);
			while (lineBegin != std::string::npos)
			{
				size_t lineEnd = source.find_first_of("\r\n", lineBegin);
				std::string line = source.substr(lineBegin, lineEnd - lineBegin);

				if (line.find(buffToken) != 0)
				{
					if (cbuffOpen)
					{
						cbuffOpen = false;
						outStr.append("}\n");
						m_constBuffers.push_back(std::make_shared<D3D11ConstantBuffer>(type, currOffset, lastSlot));
					}

					outStr.append(line);
					outStr.append("\r\n");
				}
				else
				{
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

					std::string slotStr = line.substr(slotBegin, slotEnd - slotBegin);
					std::string typeStr = line.substr(typeBegin, typeEnd - typeBegin);
					std::string nameStr = line.substr(nameBegin, nameEnd - nameBegin);

					uint32_t slot = std::stoi(slotStr);

					if (slot != lastSlot)
					{
						++currIndex;

						if (cbuffOpen)
						{
							outStr.append("}\n");
							m_constBuffers.push_back(std::make_shared<D3D11ConstantBuffer>(type, currOffset, lastSlot));
						}

						lastSlot = slot;
						
						outStr.append(
							"cbuffer _CONSTANT_BUFFER_" + slotStr +
							" : register(b" + slotStr + ")\n{\n"
						);

						cbuffOpen = true;

						currOffset = 0;
					}

					outStr.push_back('\t');
					outStr.append(typeStr + " " + nameStr + ";\n");

					ConstBufferElement element;
					element.bufferIndex = currIndex;
					element.offset = currOffset;
					element.size = ShaderDataTypeSize(hlslStringToShaderDataType(typeStr));
					m_uniforms[nameStr] = element;

					currOffset += element.size;
				}

				lineBegin = source.find_first_not_of("\r\n", lineEnd);
			}

			shader.second = outStr;
		}
	}
}