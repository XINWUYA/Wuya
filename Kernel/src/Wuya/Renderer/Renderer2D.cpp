#include "pch.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>

#include "UniformBuffer.h"
#include "Wuya/Application/OrthographicCamera.h"

namespace Wuya
{
	// Quad Vertex Positions
	const glm::vec4 QuadVertexPositions[4] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f }
	};

	struct CameraData
	{
		glm::mat4 ViewProjectionMatrix;
	};

	struct QuadVertexData
	{
		glm::vec3 Position;
		glm::vec2 TextureCoords;
		glm::vec4 Color;
		float TextureIndex;
		float TilingFactor;

		// Editor only
		int EntityId;
	};

	struct RenderData2D
	{
		static constexpr uint32_t MaxQuadNum{ 20000 };
		static constexpr uint32_t MaxVertices{ MaxQuadNum * 4 };
		static constexpr uint32_t MaxIndices{ MaxQuadNum * 6 };
		static constexpr uint32_t MaxTextureSlots{ 32 };

		SharedPtr<VertexArray> pVertexArray;
		SharedPtr<Shader> pShader;

		QuadVertexData* pQuadVertexBufferBase{ nullptr };
		QuadVertexData* pQuadVertexBufferCurrent{ nullptr };
		uint32_t TotalIndexCount{ 0 };

		SharedPtr<Texture2D> pDefaultTexture;
		std::array<SharedPtr<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 : Default Texture

		CameraData CameraData;
		SharedPtr<UniformBuffer> pCameraUniformBuffer;
	};

	static RenderData2D m_RenderData2D;

	void Renderer2D::Init()
	{
		// Vertex Array
		m_RenderData2D.pVertexArray = VertexArray::Create();

		const auto vertex_buffer = VertexBuffer::Create(m_RenderData2D.MaxVertices * sizeof(QuadVertexData));
		const VertexBufferLayout vertex_buffer_layout = {
			{ "a_Position",			BufferDataType::Float3 },
			{ "a_TexCoord",			BufferDataType::Float2 },
			{ "a_Color",			BufferDataType::Float4 },
			{ "a_TextureIndex",		BufferDataType::Float },
			{ "a_TilingFactor",		BufferDataType::Float },
			{ "a_EntityId",			BufferDataType::Int },
		};
		vertex_buffer->SetLayout(vertex_buffer_layout);
		m_RenderData2D.pVertexArray->AddVertexBuffer(vertex_buffer);

		m_RenderData2D.pQuadVertexBufferBase = new QuadVertexData[m_RenderData2D.MaxVertices];
		m_RenderData2D.pQuadVertexBufferCurrent = m_RenderData2D.pQuadVertexBufferBase;

		uint32_t* quad_indices = new uint32_t[m_RenderData2D.MaxIndices];
		for (uint32_t offset = 0, i = 0; i < m_RenderData2D.MaxIndices; i += 6)
		{
			quad_indices[i + 0] = offset + 0;
			quad_indices[i + 1] = offset + 1;
			quad_indices[i + 2] = offset + 2;

			quad_indices[i + 3] = offset + 2;
			quad_indices[i + 4] = offset + 3;
			quad_indices[i + 5] = offset + 0;

			offset += 4;
		}
		auto index_buffer = IndexBuffer::Create(quad_indices, m_RenderData2D.MaxIndices);
		m_RenderData2D.pVertexArray->SetIndexBuffer(index_buffer);
		delete[] quad_indices;

		// Shader
		m_RenderData2D.pShader = Shader::Create("assets/shaders/texture.glsl");

		// Texture
		m_RenderData2D.pDefaultTexture = Texture2D::Create(1, 1);
		uint32_t default_texture_data = 0xffffffff; // White
		m_RenderData2D.pDefaultTexture->SetData(&default_texture_data, sizeof(uint32_t));

		m_RenderData2D.TextureSlots[0] = m_RenderData2D.pDefaultTexture;

		// Uniform Buffer
		m_RenderData2D.pCameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
	}

	void Renderer2D::Update()
	{
	}

	void Renderer2D::Release()
	{
		if (m_RenderData2D.pQuadVertexBufferBase)
			delete[] m_RenderData2D.pQuadVertexBufferBase;
	}

	void Renderer2D::Flush()
	{
		if (m_RenderData2D.TotalIndexCount == 0)
			return;

		const auto data_size = (uint32_t)((uint8_t*)m_RenderData2D.pQuadVertexBufferCurrent - (uint8_t*)m_RenderData2D.pQuadVertexBufferBase);
		m_RenderData2D.pVertexArray->GetVertexBuffers()[0]->SetData(m_RenderData2D.pQuadVertexBufferBase, data_size);

		for (auto i = 0; i < m_RenderData2D.TextureSlotIndex; ++i)
			m_RenderData2D.TextureSlots[i]->Bind(i);

		m_RenderData2D.pShader->Bind();
		Renderer::Submit(m_RenderData2D.pShader, m_RenderData2D.pVertexArray, m_RenderData2D.TotalIndexCount);
	}

	void Renderer2D::BeginScene(SharedPtr<OrthographicCamera> camera)
	{
		m_RenderData2D.CameraData.ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		m_RenderData2D.pCameraUniformBuffer->SetData(&m_RenderData2D.CameraData, sizeof(CameraData));
	}

	void Renderer2D::EndScene()
	{
		Flush();
		m_RenderData2D.pQuadVertexBufferCurrent = m_RenderData2D.pQuadVertexBufferBase;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position.x, position.y, 0.0f), size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.0f));
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id)
	{
		constexpr glm::vec2 texture_coords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (size_t i = 0; i < 4; ++i)
		{
			m_RenderData2D.pQuadVertexBufferCurrent->Position = transform * QuadVertexPositions[i];
			m_RenderData2D.pQuadVertexBufferCurrent->TextureCoords = texture_coords[i];
			m_RenderData2D.pQuadVertexBufferCurrent->Color = color;
			m_RenderData2D.pQuadVertexBufferCurrent->TextureIndex = 0.0f; // Default Texture
			m_RenderData2D.pQuadVertexBufferCurrent->TilingFactor = 1.0f;
			m_RenderData2D.pQuadVertexBufferCurrent->EntityId = entity_id;

			m_RenderData2D.pQuadVertexBufferCurrent++;
		}

		m_RenderData2D.TotalIndexCount += 6;
	}
}
