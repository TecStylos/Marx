#pragma once

#include "Marx/Renderer/Buffer.h"

#include <cstdint>

namespace Marx
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();
	public:
		virtual void bind() const override;
		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override { return *m_pBufferLayout; }
		virtual void update(const void* vertices) override;
		virtual void updatePartial(const void* const* pVertices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) override;
	private:
		uint32_t m_buffer;
		uint32_t m_size;
		uint32_t m_stride;
		Scope<BufferLayout> m_pBufferLayout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, PrimitiveType primType);
		virtual ~OpenGLIndexBuffer();
	public:
		virtual void bind() const override;
		virtual void setCount(uint32_t count) override { m_count = count; }
		virtual uint32_t getCount() const override { return m_count; }
		virtual uint32_t getMaxCount() const override { return m_maxCount; }
		virtual void update(const uint32_t* indices) override;
		virtual void updatePartial(const uint32_t* const* pIndices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) override;
	private:
		uint32_t m_count;
		uint32_t m_maxCount;
		uint32_t m_buffer;
	};
}