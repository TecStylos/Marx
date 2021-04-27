#include "mxpch.h"
#include "D3D11Buffer.h"

#include "D3D11GraphicsContext.h"
#include "Marx/Platform/D3D11/D3D11InfoManager.h"
#include "Marx/Platform/D3D11/D3D11InfoException.h"
#include "Marx/Platform/D3D11/D3D11ExceptionMacros.h"

#include "Marx/Platform/D3D11/D3D11Shader.h"

namespace Marx
{
	D3D11VertexBuffer::D3D11VertexBuffer(const void* vertices, uint32_t size)
		: m_size(size), m_stride(0)
	{
		MX_DEBUG_HR_DECL;

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = size;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vbInitData;
		vbInitData.pSysMem = vertices;
		vbInitData.SysMemPitch = 0;
		vbInitData.SysMemSlicePitch = 0;
		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateBuffer(
				&vertexBufferDesc, &vbInitData,
				m_pVertexBuffer.GetAddressOf()
			)
		);
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{}

	void D3D11VertexBuffer::bind() const
	{
		uint32_t offset = 0;
		D3D11GraphicsContext::D3D11Manager::getContext()->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
		D3D11GraphicsContext::D3D11Manager::getContext()->IASetInputLayout(m_pInputLayout.Get());
	}

	void D3D11VertexBuffer::setLayout(const BufferLayout& layout)
	{
		MX_DEBUG_HR_DECL;

		auto elementDesc = genElementDesc(layout);
		ComPtr<ID3DBlob> dummyShader = genDummyShader(layout);

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateInputLayout(
				elementDesc.data(), (UINT)elementDesc.size(),
				dummyShader->GetBufferPointer(), dummyShader->GetBufferSize(),
				m_pInputLayout.GetAddressOf()
			)
		);
		m_stride = layout.getStride();

		m_pBufferLayout = std::make_unique<BufferLayout>(layout);
	}

	void D3D11VertexBuffer::update(const void* vertices)
	{
		uint32_t offset = 0;
		uint32_t count = m_size / m_stride;
		updatePartial(&vertices, &offset, &count, 1);
	}

	void D3D11VertexBuffer::updatePartial(const void* const* pVertices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers)
	{
		MX_DEBUG_HR_DECL;

		MX_CORE_ASSERT(m_stride, "Vertex stride not set!");

		auto context = D3D11GraphicsContext::D3D11Manager::getContext();
		D3D11_MAPPED_SUBRESOURCE resource;

		MX_VERIFY_THROW_HR(context->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource));

		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			memcpy(
				(char*)resource.pData + (pOffset[i] * (size_t)m_stride), 
				pVertices[i],
				pCount[i] * (size_t)m_stride
			);
		}

		context->Unmap(m_pVertexBuffer.Get(), 0);
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> D3D11VertexBuffer::genElementDesc(const BufferLayout& layout)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> elementDesc;
		for (auto& element : layout)
		{
			elementDesc.push_back(
				{ element.name.c_str(), 0, shaderDataTypeToDXGI_FORMAT(element.type), 0, element.offset, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			);
		}
		return elementDesc;
	}

	ID3DBlob* D3D11VertexBuffer::genDummyShader(const BufferLayout& layout)
	{
		MX_DEBUG_HR_DECL;

		std::string shaderSrc;
		shaderSrc.append("struct VS_INPUT {\n");
		uint32_t index = 0;
		for (auto& element : layout)
		{
			shaderSrc.append(shaderDataTypeToHLSL_String(element.type));
			shaderSrc.append(" elem" + std::to_string(index) + " : " + element.name + ";\n");
			++index;
		}
		shaderSrc.append("};\n");
		shaderSrc.append("float4 main(VS_INPUT inp) : SV_POSITION { return float4(0.0f, 0.0f, 0.0f, 0.0f); }");

		ID3DBlob* pShader;
		MX_VERIFY_THROW_HR_INFO(D3D11Shader::compileSrc(shaderSrc, "vs_5_0", &pShader));
		return pShader;
	}

	///////////////////////////////////////
	// ---------- IndexBuffer ---------- //
	///////////////////////////////////////
	
	// [indices]: Array of indices to be copied into GPU mem. If set to nullptr the buffer will be created but not initialized
	// [count]: Describes the maximum count of indices (if [indices] == nullptr) or the number of indices in [indices] (if [indices] != nullptr)
	DX11IndexBuffer::DX11IndexBuffer(const uint32_t* indices, uint32_t count, PrimitiveType primType)
		: m_count(count), m_maxCount(count)
	{
		MX_DEBUG_HR_DECL;

		if (!indices)
			m_count = 0;

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = indices;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;
		auto pInitData = indices ? &initData : nullptr;

		MX_VERIFY_THROW_HR_INFO(
			D3D11GraphicsContext::D3D11Manager::getDevice()->CreateBuffer(
				&indexBufferDesc, pInitData, m_pIndexBuffer.GetAddressOf()
			)
		);

		m_primitiveTopology = primitiveTypeToD3D11PrimitiveTopology(primType);
	}

	DX11IndexBuffer::~DX11IndexBuffer()
	{}

	void DX11IndexBuffer::bind() const
	{
		D3D11GraphicsContext::D3D11Manager::getContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		D3D11GraphicsContext::D3D11Manager::getContext()->IASetPrimitiveTopology(m_primitiveTopology);
	}

	void DX11IndexBuffer::update(const uint32_t* indices)
	{
		uint32_t offset = 0;
		uint32_t count = getMaxCount();
		updatePartial(&indices, &offset, &count, 1);
	}

	void DX11IndexBuffer::updatePartial(const uint32_t* const* pIndices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers)
	{
		MX_DEBUG_HR_DECL;

		auto context = D3D11GraphicsContext::D3D11Manager::getContext();
		D3D11_MAPPED_SUBRESOURCE resource;

		MX_VERIFY_THROW_HR(context->Map(m_pIndexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource));

		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			memcpy(
				(char*)resource.pData + pOffset[i] * sizeof(uint32_t),
				pIndices[i],
				pCount[i] * sizeof(uint32_t)
			);
		}

		context->Unmap(m_pIndexBuffer.Get(), 0);
	}
}