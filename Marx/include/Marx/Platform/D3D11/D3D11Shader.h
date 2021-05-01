#pragma once

#include <string>

#include "Marx/Renderer/Shader.h"

#include "Marx/Platform/DX11/DX11Includes.h"
#include "Marx/Platform/Win32/Win32Includes.h"

#include "D3D11ConstantBuffers.h"

namespace Marx
{
	class D3D11Shader : public Shader
	{
		struct InternalConstBuffElement
		{
			uint32_t slot;
			ShaderDataType type;
			std::string name;
		};
	private:
		typedef std::unordered_map<ShaderType, std::string> ShaderSources;
	public:
		D3D11Shader(const std::string& filepath, const std::string& name);
		D3D11Shader(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc);
		~D3D11Shader();
	public:
		virtual void bind() const override;
		virtual const std::string& getName() const override { return m_name; }
		virtual void updateUniform(const std::string& name, const void* data, ShaderDataType sdt) override;
	public:
		static HRESULT compileSrc(const std::string& src, const char* pTarget, ID3DBlob** ppBytecode);
	private:
		void compile(const ShaderSources& shaderSources);
		void detectConstBuffers(ShaderSources& shaderSources);
		InternalConstBuffElement extractElementFromLine(const std::string& line);
	private:
		static const char* getTargetStringFromType(ShaderType type);
	private:
		std::string m_name;
		ComPtr<ID3D11PixelShader> m_pPixelShader;
		ComPtr<ID3D11VertexShader> m_pVertexShader;
		struct ConstBufferElement
		{
			uint32_t bufferIndex;
			uint32_t offset;
			uint32_t size;
		};
		std::unordered_map<std::string, ConstBufferElement> m_uniforms;
		std::vector<Reference<D3D11ConstantBuffer>> m_constBuffers;
	};
}