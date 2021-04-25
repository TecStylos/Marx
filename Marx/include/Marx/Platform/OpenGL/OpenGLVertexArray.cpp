#include "mxpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Marx
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Marx::ShaderDataType::Float1: return GL_FLOAT;
		case Marx::ShaderDataType::Float2: return GL_FLOAT;
		case Marx::ShaderDataType::Float3: return GL_FLOAT;
		case Marx::ShaderDataType::Float4: return GL_FLOAT;
		case Marx::ShaderDataType::Mat3:   return GL_FLOAT;
		case Marx::ShaderDataType::Mat4:   return GL_FLOAT;
		case Marx::ShaderDataType::Int1:   return GL_INT;
		case Marx::ShaderDataType::Int2:   return GL_INT;
		case Marx::ShaderDataType::Int3:   return GL_INT;
		case Marx::ShaderDataType::Int4:   return GL_INT;
		case Marx::ShaderDataType::Bool1:  return GL_BOOL;
		}

		MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_array);
	}

	OpenGLVertexArray::OpenGLVertexArray(const Reference<VertexBuffer>& pVertexBuffer, const Reference<IndexBuffer>& pIndexBuffer)
		: OpenGLVertexArray()
	{
		setVertexBuffer(pVertexBuffer);
		setIndexBuffer(pIndexBuffer);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_array);
	}

	void OpenGLVertexArray::bind() const
	{
		glBindVertexArray(m_array);
	}

	void OpenGLVertexArray::setVertexBuffer(const Reference<VertexBuffer>& vertexBuffer)
	{
		const auto& layout = vertexBuffer->getLayout();
		MX_CORE_ASSERT(layout.getElements().size(), "Vertex buffer has no layout!");

		bind();
		vertexBuffer->bind();

		for (int i = 0; i < m_vertexBufferIndex; ++i)
			glDisableVertexAttribArray(i);
		m_vertexBufferIndex = 0;
		
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_vertexBufferIndex);
			glVertexAttribPointer(
				m_vertexBufferIndex,
				element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)(intptr_t)element.offset);
			++m_vertexBufferIndex;
		}

		m_pVertexBuffer = vertexBuffer;
	}

	void OpenGLVertexArray::setIndexBuffer(const Reference<IndexBuffer>& indexBuffer)
	{
		bind();
		indexBuffer->bind();
		m_pIndexBuffer = indexBuffer;
	}

	Reference<VertexBuffer> OpenGLVertexArray::getVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	Reference<IndexBuffer> OpenGLVertexArray::getIndexBuffer()
	{
		return m_pIndexBuffer;
	}
}