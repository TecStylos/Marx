#include "mxpch.h"

#include "D3D11Shader.h"

#include "Marx/Platform/D3D11/D3D11InfoManager.h"
#include "Marx/Platform/D3D11/D3D11InfoException.h"
#include "Marx/Platform/D3D11/D3D11ExceptionMacros.h"

#include "Marx/Platform/D3D11/D3D11GraphicsContext.h"

namespace Marx
{
	D3D11Shader::D3D11Shader(const std::string& filepath)
	{
		auto shaderSrc = readFile(filepath);
		auto sources = preprocess(shaderSrc);
		compile(sources);
	}

	D3D11Shader::D3D11Shader(const std::string& vertexSrc, const std::string& pixelSrc)
	{
		MX_DEBUG_HR_DECL;

		ComPtr<ID3DBlob> pVSCode, pPSCode;
		MX_VERIFY_THROW_HR_INFO(compileSrc(vertexSrc, getTargetStringFromType(ShaderType::Vertex), pVSCode.GetAddressOf()));
		MX_VERIFY_THROW_HR_INFO(compileSrc(pixelSrc, getTargetStringFromType(ShaderType::Pixel), pPSCode.GetAddressOf()));
		MX_VERIFY_THROW_HR_INFO(D3D11GraphicsContext::D3D11Manager::getDevice()->CreateVertexShader(pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), NULL, m_pVertexShader.GetAddressOf()));
		MX_VERIFY_THROW_HR_INFO(D3D11GraphicsContext::D3D11Manager::getDevice()->CreatePixelShader(pPSCode->GetBufferPointer(), pPSCode->GetBufferSize(), NULL, m_pPixelShader.GetAddressOf()));
	}

	D3D11Shader::~D3D11Shader()
	{}

	void D3D11Shader::bind() const
	{
		D3D11GraphicsContext::D3D11Manager::getContext()->VSSetShader(m_pVertexShader.Get(), 0, 0);
		D3D11GraphicsContext::D3D11Manager::getContext()->PSSetShader(m_pPixelShader.Get(), 0, 0);
	}

	const char* D3D11Shader::getTargetStringFromType(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Vertex: return "vs_5_0";
		case ShaderType::Pixel: return "ps_5_0";
		default: return "Unknown Shader";
		}
	}

	std::unordered_map<ShaderType, std::string> D3D11Shader::preprocess(std::string shaderSrc)
	{
		std::unordered_map<ShaderType, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = shaderSrc.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = shaderSrc.find_first_of("\r\n", pos);
			MX_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = shaderSrc.substr(begin, eol - begin);
			MX_CORE_ASSERT(type == "vertex" || type == "pixel", "Invalid shader type");

			size_t nextLinePos = shaderSrc.find_first_not_of("\r\n", eol);
			pos = shaderSrc.find(typeToken, nextLinePos);
			shaderSources[shaderTypeFromString(type)] = shaderSrc.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? shaderSrc.size() - 1 : nextLinePos));
		}

		return shaderSources;
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

	void D3D11Shader::compile(const std::unordered_map<ShaderType, std::string> shaderSources)
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
}