#include "mxpch.h"
#include "D3D11ConstantBuffers.h"

#include "D3D11GraphicsContext.h"
#include "D3D11InfoManager.h"
#include "D3D11InfoException.h"
#include "D3D11ExceptionMacros.h"

namespace Marx
{
	D3D11ConstantBuffer::D3D11ConstantBuffer(ShaderType shaderType, uint32_t size, uint32_t slot)
		: m_type(shaderType), m_size(size), m_slot(slot), m_stagedData(new char[size])
	{
		switch (m_type)
		{
		case ShaderType::Pixel:
			m_bindFunc = &ID3D11DeviceContext::PSSetConstantBuffers;
			break;
		case ShaderType::Vertex:
			m_bindFunc = &ID3D11DeviceContext::VSSetConstantBuffers;
			break;
		default:
			MX_CORE_ASSERT(false, "Unknown ShaderType!");
		}

		MX_DEBUG_HR_DECL;

		MX_CORE_ASSERT(size % 16 == 0, "The size must be a multiple of 16");

		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = size;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = m_stagedData;
		D3D11_SUBRESOURCE_DATA* pSd = m_stagedData ? &sd : nullptr;

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateBuffer(
				&bd, pSd, m_pBuffer.GetAddressOf()
			)
		);
	}

	D3D11ConstantBuffer::~D3D11ConstantBuffer()
	{
		delete[] m_stagedData;
	}

	void D3D11ConstantBuffer::bind() const
	{
		if (m_staged)
			copyStaged();
		auto context = D3D11GraphicsContext::D3D11Manager::getContext();
		((*context).*m_bindFunc)(m_slot, 1, m_pBuffer.GetAddressOf());
	}

	void D3D11ConstantBuffer::update(const void* data)
	{
		updatePartial(data, 0, m_size);
	}

	void D3D11ConstantBuffer::updatePartial(const void* data, uint32_t offset, uint32_t size)
	{
		updatePartialStaged(data, offset, size);
		copyStaged();
	}

	void D3D11ConstantBuffer::updateStaged(const void* data)
	{
		updatePartialStaged(data, 0, m_size);
	}

	void D3D11ConstantBuffer::updatePartialStaged(const void* data, uint32_t offset, uint32_t size)
	{
		memcpy(m_stagedData + offset, data, size);
		m_staged = true;
	}

	void D3D11ConstantBuffer::copyStaged() const
	{
		D3D11GraphicsContext::D3D11Manager::getContext()->UpdateSubresource(m_pBuffer.Get(), 0, NULL, m_stagedData, 0, 0);
		m_staged = false;
	}
}