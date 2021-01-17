#pragma once

#include "Marx/Renderer/VertexArray.h"

namespace Marx
{
	class D3D11VertexArray : public VertexArray
	{
	public:
		D3D11VertexArray();
		D3D11VertexArray(const Reference<VertexBuffer>& pVertexBuffer, const Reference<IndexBuffer>& pIndexBuffer);
		virtual ~D3D11VertexArray() {}
	public:
		virtual void bind() const override;
		virtual void setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const Reference<IndexBuffer>& indexBuffer) override;
		virtual Reference<VertexBuffer> getVertexBuffer() override;
		virtual Reference<IndexBuffer> getIndexBuffer() override;
	private:
		Reference<VertexBuffer> m_pVertexBuffer;
		Reference<IndexBuffer> m_pIndexBuffer;
	};
}