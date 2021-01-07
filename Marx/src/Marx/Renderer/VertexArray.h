#pragma once

#include "Buffer.h"

namespace Marx
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}
	public:
		virtual void bind() const = 0;
		virtual void setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const Reference<IndexBuffer>& indexBuffer) = 0;
		virtual VertexBuffer* getVertexBuffer() = 0;
		virtual IndexBuffer* getIndexBuffer() = 0;
	public:
		static VertexArray* create();
	};
}