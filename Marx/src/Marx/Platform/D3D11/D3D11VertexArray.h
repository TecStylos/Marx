#pragma once

#include "Marx/Renderer/VertexArray.h"

namespace Marx
{
	class D3D11VertexArray : public VertexArray
	{
	public:
		D3D11VertexArray();
		virtual ~D3D11VertexArray() {}
	public:
		virtual void bind() const override;
		virtual void setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const Reference<IndexBuffer>& indexBuffer) override;
		virtual VertexBuffer* getVertexBuffer() override;
		virtual IndexBuffer* getIndexBuffer() override;
	private:
		Reference<VertexBuffer> m_pVertexBuffer;
		Reference<IndexBuffer> m_pIndexBuffer;
	};
}