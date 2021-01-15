#pragma once

#include <string>

#include "Marx/Renderer/Shader.h"

#include "Marx/Platform/DX11/DX11Includes.h"
#include "Marx/Platform/Win32/Win32Includes.h"

namespace Marx
{
	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(const std::string& filepath);
		D3D11Shader(const std::string& vertexSrc, const std::string& pixelSrc);
		~D3D11Shader();
	public:
		virtual void bind() const override;
	public:
		static HRESULT compileSrc(const std::string& src, const char* pTarget, ID3DBlob** ppBytecode);
	private:
		void compile(const std::unordered_map<ShaderType, std::string> shaderSources);
	private:
		static const char* getTargetStringFromType(ShaderType type);
		static std::unordered_map<ShaderType, std::string> preprocess(std::string shaderSrc);
	private:
		ComPtr<ID3D11PixelShader> m_pPixelShader;
		ComPtr<ID3D11VertexShader> m_pVertexShader;
	};
}