#pragma once

#include "Marx/Renderer/Shader.h"

#include "Marx/Platform/DX11/DX11Includes.h"
#include "Marx/Platform/Win32/Win32Includes.h"

namespace Marx
{
	class D3D11ConstantBuffer
	{
	public:
		D3D11ConstantBuffer(ShaderType shaderType, uint32_t size, uint32_t slot);
		~D3D11ConstantBuffer();
	public:
		void bind() const;
		void update(const void* data);
		void updatePartial(const void* data, uint32_t offset, uint32_t size);
		void updateStaged(const void* data);
		void updatePartialStaged(const void* data, uint32_t offset, uint32_t size);
	private:
		void copyStaged() const;
	private:
		ShaderType m_type;
		typedef void(__stdcall ID3D11DeviceContext::*BindFunc)(UINT, UINT, ID3D11Buffer* const*);
		BindFunc m_bindFunc = nullptr;
		uint32_t m_size;
		uint32_t m_slot;
		mutable bool m_staged = false;
		char* const m_stagedData;
		ComPtr<ID3D11Buffer> m_pBuffer;
	};
}