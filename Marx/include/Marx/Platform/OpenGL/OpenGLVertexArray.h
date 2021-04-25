#pragma once

#include "Marx/Renderer/VertexArray.h"

namespace Marx
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		OpenGLVertexArray(const Reference<VertexBuffer>& pVertexBuffer, const Reference<IndexBuffer>& pIndexBuffer);
		virtual ~OpenGLVertexArray();
	public:
		virtual void bind() const override;
		virtual void setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const Reference<IndexBuffer>& indexBuffer) override;
		virtual Reference<VertexBuffer> getVertexBuffer() override;
		virtual Reference<IndexBuffer> getIndexBuffer() override;
	private:
		uint32_t m_array;
		uint32_t m_vertexBufferIndex = 0;
		Reference<VertexBuffer> m_pVertexBuffer;
		Reference<IndexBuffer> m_pIndexBuffer;
	};
}