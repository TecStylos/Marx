#include "mxpch.h"
#include "D3D11VertexArray.h"

namespace Marx
{
	D3D11VertexArray::D3D11VertexArray()
	{}

	D3D11VertexArray::D3D11VertexArray(const Reference<VertexBuffer>& pVertexBuffer, const Reference<IndexBuffer>&pIndexBuffer)
	{
		setVertexBuffer(pVertexBuffer);
		setIndexBuffer(pIndexBuffer);
	}

	void D3D11VertexArray::bind() const
	{
		m_pVertexBuffer->bind();
		m_pIndexBuffer->bind();
	}

	void D3D11VertexArray::setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer)
	{
		m_pVertexBuffer = vertexBuffer;
	}

	void D3D11VertexArray::setIndexBuffer(const Reference<IndexBuffer>& indexBuffer)
	{
		m_pIndexBuffer = indexBuffer;
	}

	Reference<VertexBuffer> D3D11VertexArray::getVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	Reference<IndexBuffer> D3D11VertexArray::getIndexBuffer()
	{
		return m_pIndexBuffer;
	}
}