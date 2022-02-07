#pragma once
#include <string>

namespace Wuya
{
	enum class BufferDataType
	{
		None = 0,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4
	};

	struct BufferElement
	{
		std::string Name;
		BufferDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;
		BufferElement(const std::string& name, BufferDataType type, bool normalized = false);

		uint32_t GetComponentCount() const;
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(std::initializer_list<BufferElement> elements);

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

		std::vector<BufferElement> m_Elements{};
		uint32_t m_Stride{ 0 };
	};

	// Vertex Buffer
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const VertexBufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		virtual uint32_t GetCount() const = 0;

		static SharedPtr<VertexBuffer> Create(uint32_t size);
		static SharedPtr<VertexBuffer> Create(const float* vertices, uint32_t size);
	};

	// Index Buffer
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static SharedPtr<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
	};
}
