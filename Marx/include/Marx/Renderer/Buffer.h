#pragma once

#include "Shader.h"

namespace Marx
{
	class BufferLayout
	{
	public:
		struct Element;
	public:
		BufferLayout(const std::initializer_list<Element>& elements)
			: m_elements(elements)
		{
			calculateElementData();
		}
	public:
		inline const std::vector<Element>& getElements() const { return m_elements; }
		inline uint32_t getElementCount() const { return (uint32_t)m_elements.size(); }
		inline uint32_t getStride() const { return m_stride; }
	public:
		std::vector<Element>::iterator begin() { return m_elements.begin(); }
		std::vector<Element>::iterator end() { return m_elements.end(); }
		std::vector<Element>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<Element>::const_iterator end() const { return m_elements.end(); }
	private:
		void calculateElementData();
	private:
		std::vector<Element> m_elements;
		uint32_t m_stride;
	public:
		struct Element
		{
			std::string name;
			ShaderDataType type;
			uint32_t size;
			uint32_t offset;
			uint32_t componentCount;
			bool normalized;
		public:
			Element(ShaderDataType Type, const std::string& Name, bool normalized = false)
				: type(Type), name(Name), size(ShaderDataTypeSize(Type)), offset(0), normalized(normalized)
			{}
		public:
			uint32_t getComponentCount() const
			{
				switch (type)
				{
				case ShaderDataType::None: MX_CORE_ASSERT(false, "ShaderDataType::None is not supported!"); break;
				case ShaderDataType::Float1: return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Mat3:   return 3 * 3;
				case ShaderDataType::Mat4:   return 4 * 4;
				case ShaderDataType::Int1:   return 1;
				case ShaderDataType::Int2:   return 2;
				case ShaderDataType::Int3:   return 3;
				case ShaderDataType::Int4:   return 4;
				case ShaderDataType::Bool1:  return 1;
				}

				MX_CORE_ASSERT(false, "Unknown ShaderDataType!");
				return 0;
			}
		};
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		virtual void bind() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout() const = 0;
		// Updates the whole vertex buffer
		//
		// @param vertices Array of vertices (The size of the buffer must be equal to the size specified on creation)
		virtual void update(const void* vertices) = 0;
		// Updates one or more parts of the underlying vertex buffer
		//
		// @param pVertices Array of vertex buffers
		// @param pOffset Array of offsets in the destination buffer (in vertices not bytes)
		// @param pCount Array of the number of vertices in each buffer
		// @param nBuffers The number of buffers in pVertices
		virtual void updatePartial(const void* const* pVertices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) = 0;
	public:
		static Reference<VertexBuffer> create(const void* vertices, uint32_t size);
	};

	///////////////////////////////////////
	// ---------- IndexBuffer ---------- //
	///////////////////////////////////////

	class IndexBuffer
	{
	public:
		enum class PrimitiveType
		{
			None = 0, PointList,
			LineList, LineStrip,
			TriangleList, TriangleStrip
		};
	public:
		virtual ~IndexBuffer() {}
		virtual void bind() const = 0;
		virtual void setCount(uint32_t count) = 0;
		virtual uint32_t getCount() const = 0;
		virtual uint32_t getMaxCount() const = 0;
		// Updates the whole index buffer
		//
		// @param indices Array of indices (Number of elements must be equal to getCount())
		virtual void update(const uint32_t* indices) = 0;
		// Updates one or more parts of the underlying index buffer
		//
		// @param pIndices Array of index buffers
		// @param pOffset Array of offsets in the destination buffer (in indices not bytes)
		// @param pCount Array of the number of indices in each buffer
		// @param nBuffers The number of buffers in pIndices
		virtual void updatePartial(const uint32_t* const* pIndices, const uint32_t* pOffset, const uint32_t* pCount, uint32_t nBuffers) = 0;
	public:
		static Reference<IndexBuffer> create(const uint32_t* indices, uint32_t count, PrimitiveType primType);
	};
}