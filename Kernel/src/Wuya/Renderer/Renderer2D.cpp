#include "Pch.h"
#include "Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "UniformBuffer.h"
#include "Wuya/Application/AssetManager.h"
#include "Wuya/Scene/Components.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/Mesh.h"

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
		static constexpr uint32_t MaxVertexNum{ MaxQuadNum * 4 };
		static constexpr uint32_t MaxIndexNum{ MaxQuadNum * 6 };
		static constexpr uint32_t MaxTextureSlotNum{ 32 };

		SharedPtr<VertexArray> pVertexArray;
		SharedPtr<Shader> pShader;

		QuadVertexData* pQuadVertexBufferBase{ nullptr };
		QuadVertexData* pQuadVertexBufferCurrent{ nullptr };
		uint32_t TotalIndexCount{ 0 };

		std::array<SharedPtr<Texture>, MaxTextureSlotNum> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 : Default Texture

		CameraData CameraData;
		SharedPtr<UniformBuffer> pCameraUniformBuffer;
	};

	// Renderer2D 相关数据
	static RenderData2D s_RenderData2D;
	// Renderer2D 统计信息
	static Renderer2D::StatisticsInfo s_StatisticsInfo;

	// ***************************************************************************************
	// Renderer2D
	void Renderer2D::Init()
	{
		PROFILE_FUNCTION();

		// Vertex Array
		s_RenderData2D.pVertexArray = VertexArray::Create();

		const auto vertex_buffer = VertexBuffer::Create(s_RenderData2D.MaxVertexNum * sizeof(QuadVertexData));
		const VertexBufferLayout vertex_buffer_layout = {
			{ "a_Position",			BufferDataType::Float3 },
			{ "a_TexCoord",			BufferDataType::Float2 },
			{ "a_Color",			BufferDataType::Float4 },
			{ "a_TextureIndex",		BufferDataType::Float },
			{ "a_TilingFactor",		BufferDataType::Float },
			{ "a_EntityId",			BufferDataType::Int },
		};
		vertex_buffer->SetLayout(vertex_buffer_layout);
		s_RenderData2D.pVertexArray->AddVertexBuffer(vertex_buffer);

		s_RenderData2D.pQuadVertexBufferBase = new QuadVertexData[s_RenderData2D.MaxVertexNum];
		s_RenderData2D.pQuadVertexBufferCurrent = s_RenderData2D.pQuadVertexBufferBase;

		uint32_t* quad_indices = new uint32_t[s_RenderData2D.MaxIndexNum];
		for (uint32_t offset = 0, i = 0; i < s_RenderData2D.MaxIndexNum; i += 6)
		{
			quad_indices[i + 0] = offset + 0;
			quad_indices[i + 1] = offset + 1;
			quad_indices[i + 2] = offset + 2;

			quad_indices[i + 3] = offset + 2;
			quad_indices[i + 4] = offset + 3;
			quad_indices[i + 5] = offset + 0;

			offset += 4;
		}
		auto index_buffer = IndexBuffer::Create(quad_indices, s_RenderData2D.MaxIndexNum);
		s_RenderData2D.pVertexArray->SetIndexBuffer(index_buffer);
		delete[] quad_indices;

		// Shader
		s_RenderData2D.pShader = ShaderAssetManager::Instance().GetOrLoad("assets/shaders/texture.glsl");

		// Default texture
		s_RenderData2D.TextureSlots[0] = Texture::White();

		// Uniform Buffer
		s_RenderData2D.pCameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
	}

	void Renderer2D::Update()
	{
		PROFILE_FUNCTION();
	}

	void Renderer2D::Release()
	{
		PROFILE_FUNCTION();

		delete[] s_RenderData2D.pQuadVertexBufferBase;
	}

	Renderer2D::StatisticsInfo Renderer2D::GetStatisticsInfo()
	{
		return s_StatisticsInfo;
	}

	void Renderer2D::ResetStatisticsInfo()
	{
		memset(&s_StatisticsInfo, 0, sizeof(StatisticsInfo));
	}

	void Renderer2D::StartNewBatch()
	{
		PROFILE_FUNCTION();

		/* 重置新批次的渲染数据 */
		s_RenderData2D.TotalIndexCount = 0;
		s_RenderData2D.pQuadVertexBufferCurrent = s_RenderData2D.pQuadVertexBufferBase;
		s_RenderData2D.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		PROFILE_FUNCTION();

		/* 提交当前批次数据 */
		Flush();

		/* 开启新的合批 */
		StartNewBatch();
	}

	void Renderer2D::Flush()
	{
		PROFILE_FUNCTION();

		if (s_RenderData2D.TotalIndexCount == 0)
			return;

		const auto data_size = (uint32_t)((uint8_t*)s_RenderData2D.pQuadVertexBufferCurrent - (uint8_t*)s_RenderData2D.pQuadVertexBufferBase);
		s_RenderData2D.pVertexArray->GetVertexBuffers()[0]->SetData(s_RenderData2D.pQuadVertexBufferBase, data_size);

		for (uint32_t i = 0; i < s_RenderData2D.TextureSlotIndex; ++i)
			s_RenderData2D.TextureSlots[i]->Bind(i);

		s_RenderData2D.pShader->Bind();
		SharedPtr<Material> material = CreateSharedPtr<Material>();
		material->SetShader(s_RenderData2D.pShader);
		Renderer::Submit(material, s_RenderData2D.pVertexArray, s_RenderData2D.TotalIndexCount);

		s_StatisticsInfo.DrawCalls++;
	}

	void Renderer2D::BeginFrame(Camera* camera)
	{
		PROFILE_FUNCTION();

		ResetStatisticsInfo();

		s_RenderData2D.CameraData.ViewProjectionMatrix = camera->GetViewProjectionMatrix();
		s_RenderData2D.pCameraUniformBuffer->SetData(&s_RenderData2D.CameraData, sizeof(CameraData));

		StartNewBatch();
	}

	void Renderer2D::EndFrame()
	{
		PROFILE_FUNCTION();

		/* 提交最后一个批次数据 */
		Flush();
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

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const SharedPtr<Texture>& texture, const glm::vec4& color, float tiling_factor)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1), glm::vec3(size.x, size.y, 1.0f));
		DrawQuad(transform, texture, color, tiling_factor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id)
	{
		PROFILE_FUNCTION();

		/* 索引数量达到阈值时，开启新的合批 */
		if (s_RenderData2D.TotalIndexCount >= RenderData2D::MaxIndexNum)
			NextBatch();

		constexpr glm::vec2 texture_coords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		for (size_t i = 0; i < 4; ++i)
		{
			s_RenderData2D.pQuadVertexBufferCurrent->Position = transform * QuadVertexPositions[i];
			s_RenderData2D.pQuadVertexBufferCurrent->TextureCoords = texture_coords[i];
			s_RenderData2D.pQuadVertexBufferCurrent->Color = color;
			s_RenderData2D.pQuadVertexBufferCurrent->TextureIndex = 0.0f; // Default Texture
			s_RenderData2D.pQuadVertexBufferCurrent->TilingFactor = 1.0f;
			s_RenderData2D.pQuadVertexBufferCurrent->EntityId = entity_id;

			s_RenderData2D.pQuadVertexBufferCurrent++;
		}

		s_RenderData2D.TotalIndexCount += 6;

		s_StatisticsInfo.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture>& texture, const glm::vec4& color, float tiling_factor, int entity_id)
	{
		PROFILE_FUNCTION();

		/* 索引数量达到阈值时，开启新的合批 */
		if (s_RenderData2D.TotalIndexCount >= RenderData2D::MaxIndexNum)
			NextBatch();

		constexpr glm::vec2 texture_coords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		/* 判断贴图是否已经存在 */
		uint32_t texture_idx = 0;
		for (uint32_t i = 0; i < s_RenderData2D.TextureSlotIndex; ++i)
		{
			if (*s_RenderData2D.TextureSlots[i] == *texture)
			{
				texture_idx = i;
				break;
			}
		}

		/* 若不存在，则将贴图放到一个slot */
		if (texture_idx == 0)
		{
			/* 当贴图数量过多时，开启新的合批 */
			if (s_RenderData2D.TextureSlotIndex >= RenderData2D::MaxTextureSlotNum)
				NextBatch();

			texture_idx = s_RenderData2D.TextureSlotIndex;
			s_RenderData2D.TextureSlots[s_RenderData2D.TextureSlotIndex] = texture;
			s_RenderData2D.TextureSlotIndex++;
		}

		/* 填充当前Quad数据 */
		for (size_t i = 0; i < 4; ++i)
		{
			s_RenderData2D.pQuadVertexBufferCurrent->Position = transform * QuadVertexPositions[i];
			s_RenderData2D.pQuadVertexBufferCurrent->TextureCoords = texture_coords[i];
			s_RenderData2D.pQuadVertexBufferCurrent->Color = color;
			s_RenderData2D.pQuadVertexBufferCurrent->TextureIndex = static_cast<float>(texture_idx);
			s_RenderData2D.pQuadVertexBufferCurrent->TilingFactor = tiling_factor;
			s_RenderData2D.pQuadVertexBufferCurrent->EntityId = entity_id;

			s_RenderData2D.pQuadVertexBufferCurrent++;
		}

		/* 更新总索引数 */
		s_RenderData2D.TotalIndexCount += 6;
		/* 统计渲染信息 */
		s_StatisticsInfo.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteComponent* component, int entity_id)
	{
		PROFILE_FUNCTION();

		if (component->Texture)
		{
			DrawQuad(transform, component->Texture, component->BaseColor, component->TilingFactor, entity_id);
		}
		else
		{
			DrawQuad(transform, component->BaseColor, entity_id);
		}
	}
}
