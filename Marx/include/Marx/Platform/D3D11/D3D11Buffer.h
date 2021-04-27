#pragma once

#include "Marx/Renderer/Buffer.h"

#include "D3D11Conversions.h"

namespace Marx
{
	class D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(const void* vertices, uint32_t size);
		virtual ~D3D11VertexBuffer();
	public:
		virtual void bind() const override;
		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override { return *m_pBufferLayout; }
		virtual void update(const void* vertices) override;
		virtual void updatePartial(const void* const* pVertices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) override;
	public:
		static std::vector<D3D11_INPUT_ELEMENT_DESC> genElementDesc(const BufferLayout& layout);
		static ID3DBlob* genDummyShader(const BufferLayout& layout);
	private:
		uint32_t m_size;
		uint32_t m_stride;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11InputLayout> m_pInputLayout;
		Scope<BufferLayout> m_pBufferLayout;
	};

	///////////////////////////////////////
	// ---------- IndexBuffer ---------- //
	///////////////////////////////////////

	class DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(const uint32_t* indices, uint32_t count, PrimitiveType primType);
		virtual ~DX11IndexBuffer();
	public:
		virtual void bind() const override;
		virtual void setCount(uint32_t count) override { m_count = count; }
		virtual uint32_t getCount() const override { return m_count; }
		virtual uint32_t getMaxCount() const override { return m_maxCount; }
		virtual void update(const uint32_t* indices) override;
		virtual void updatePartial(const uint32_t* const* pIndices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) override;
	public:
		uint32_t m_count;
		uint32_t m_maxCount;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	};

	static D3D11_PRIMITIVE_TOPOLOGY primitiveTypeToD3D11PrimitiveTopology(IndexBuffer::PrimitiveType primType)
	{
		using PT = IndexBuffer::PrimitiveType;
		switch (primType)
		{
		case PT::None: return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case PT::PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PT::LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case PT::LineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PT::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PT::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}

		MX_CORE_ASSERT(false, "Unknown PrimitiveType!");
		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}