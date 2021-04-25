#include "mxpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Marx
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_buffer);
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_buffer);
	}

	void OpenGLVertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	}

	void OpenGLVertexBuffer::setLayout(const BufferLayout& layout)
	{
		m_stride = layout.getStride();
		m_pBufferLayout = std::make_unique<BufferLayout>(layout);
	}

	void OpenGLVertexBuffer::update(const void* vertices)
	{
		bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, vertices);
	}

	void OpenGLVertexBuffer::updatePartial(const void* const* pVertices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers)
	{
		bind();
		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			glBufferSubData(GL_ARRAY_BUFFER, pOffset[i] * m_stride, pCount[i] * m_stride, pVertices[i]);
		}
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, PrimitiveType primType)
		: m_count(count), m_maxCount(count)
	{
		glCreateBuffers(1, &m_buffer);
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_buffer);
	}

	void OpenGLIndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer);
	}

	void OpenGLIndexBuffer::update(const uint32_t* indices)
	{
		bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_count * sizeof(uint32_t), indices);
	}

	void OpenGLIndexBuffer::updatePartial(const uint32_t* const* pIndices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers)
	{
		bind();
		for (uint32_t i = 0; i < nBuffers; ++i)
		{
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, pOffset[i], pCount[i] * sizeof(uint32_t), pIndices[i]);
		}
	}
}