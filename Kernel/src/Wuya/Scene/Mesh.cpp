#include "Pch.h"
#include "Mesh.h"
#include "Wuya/Renderer/Buffer.h"
#include "Wuya/Renderer/VertexArray.h"

namespace Wuya
{
	MeshSegment::MeshSegment(const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material)
		: m_pVertexArray(vertex_array), m_pMaterial(material)
	{
	}

	/* 创建一个Cube类型的网格 */
	static SharedPtr<MeshSegment> CreateCube(const SharedPtr<Material>& material)
	{
		// Cube vertices
		static constexpr float vertices[] = {
			// Position,          TexCoord,    EntityId
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
		};

		// Vertex Array
		SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
		VertexBufferLayout vertex_buffer_layout = {
			{ "a_Position", BufferDataType::Float3 },
			{ "a_TexCoord", BufferDataType::Float2 },
			{ "a_EntityId", BufferDataType::Int}
		};
		vertex_buffer->SetLayout(vertex_buffer_layout);

		auto vertex_array = VertexArray::Create();
		vertex_array->Bind();
		vertex_array->AddVertexBuffer(vertex_buffer);

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 创建一个Sphere类型的网格 */
	static SharedPtr<MeshSegment> CreateSphere(const SharedPtr<Material>& material)
	{
		// todo: Cube vertices
		static constexpr float vertices[] = {
			// Position,          TexCoord,    EntityId
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
		};

		// Vertex Array
		SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
		VertexBufferLayout vertex_buffer_layout = {
			{ "a_Position", BufferDataType::Float3 },
			{ "a_TexCoord", BufferDataType::Float2 },
			{ "a_EntityId", BufferDataType::Int}
		};
		vertex_buffer->SetLayout(vertex_buffer_layout);

		auto vertex_array = VertexArray::Create();
		vertex_array->Bind();
		vertex_array->AddVertexBuffer(vertex_buffer);

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 创建一个Plane类型的网格 */
	static SharedPtr<MeshSegment> CreatePlane(const SharedPtr<Material>& material)
	{
		// todo: Plane vertices
		static constexpr float vertices[] = {
			// Position,          TexCoord,    EntityId
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
		};

		// Vertex Array
		SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
		VertexBufferLayout vertex_buffer_layout = {
			{ "a_Position", BufferDataType::Float3 },
			{ "a_TexCoord", BufferDataType::Float2 },
			{ "a_EntityId", BufferDataType::Int}
		};
		vertex_buffer->SetLayout(vertex_buffer_layout);

		auto vertex_array = VertexArray::Create();
		vertex_array->Bind();
		vertex_array->AddVertexBuffer(vertex_buffer);

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 根据类型创建一个常规网格 */
	SharedPtr<MeshSegment> CreatePrimitive(PrimitiveType type, const SharedPtr<Material>& material)
	{
		switch (type)
		{
		case PrimitiveType::Cube:   return CreateCube(material);
		case PrimitiveType::Sphere: return CreateSphere(material);
		case PrimitiveType::Plane:  return CreatePlane(material);
		}
	}
}
