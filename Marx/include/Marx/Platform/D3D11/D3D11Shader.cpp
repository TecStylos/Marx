#include "mxpch.h"

#include "D3D11Shader.h"

#include "Marx/Platform/D3D11/D3D11InfoManager.h"
#include "Marx/Platform/D3D11/D3D11InfoException.h"
#include "Marx/Platform/D3D11/D3D11ExceptionMacros.h"

#include "Marx/Platform/D3D11/D3D11GraphicsContext.h"

namespace Marx
{
	D3D11Shader::D3D11Shader(const std::string& filepath, const std::string& name)
		: m_name(name)
	{
		auto shaderSrc = readFile(filepath);
		auto sources = preprocess(shaderSrc);
		compile(sources);
	}

	D3D11Shader::D3D11Shader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc)
		: m_name(name)
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

	void D3D11Shader::updateConstBuff(ShaderDataType sdt, const void* data, const std::string& name)
	{
		; // TODO: Implement constant buffer update (d3d11)
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

	void D3D11Shader::compile(const std::unordered_map<ShaderType, std::string>& shaderSources)
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