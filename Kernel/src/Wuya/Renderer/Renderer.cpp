#include "Pch.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "RenderView.h"
#include "Texture.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include "Wuya/Scene/Camera.h"
#include "Wuya/Scene/Material.h"
#include "Wuya/Scene/Mesh.h"

namespace Wuya
{
	
	/* GlobalUniformBuffer的绑定位置 */
	namespace UniformBufferBindingPoint
	{
		constexpr uint8_t View			= 0;
		constexpr uint8_t Object		= 1;
		constexpr uint8_t Material		= 2;
		constexpr uint8_t Light			= 3;
	}

	/* Per view uniform data */
	struct ViewUniformData
	{
		glm::mat4 ViewMatrix{ 1 };
		glm::mat4 ProjectionMatrix{ 1 };
		glm::mat4 ViewProjectionMatrix{ 1 };
		glm::vec3 ViewPos{ 0.0f };
		uint32_t FrameCounter{ 0 }; /* 当前帧数计数 */
	};

	/* Per object uniform data */
	struct ObjectUniformData
	{
		glm::mat4 LocalToWorldMat{ 1 };	/* 模型矩阵 */
		uint32_t ObjectId{ 0 };				/* 模型ID，用于Picking */
	};

	/* Per light uniform data */
	struct LightUniformData
	{
		glm::vec4 ColorIntensity{ 1.0f, 1.0f, 1.0f, 1.0f }; /* rgb: color, a: intensity */
		glm::vec3 LightDir{ 0.0f };
		float Padding0;
		glm::vec3 LightPos{ 0.0f };
		uint32_t LightType{ 0 };
	};

	struct RenderData
	{
		/* 全局UniformBuffers */
		ViewUniformData ViewUniformData;
		SharedPtr<UniformBuffer> pViewUniformBuffer;
		SharedPtr<UniformBuffer> pObjectUniformBuffer;
		SharedPtr<UniformBuffer> pLightUniformBuffer;
	};

	static RenderData s_RenderData;

	SharedPtr<RenderAPI> Renderer::m_pRenderAPI = RenderAPI::Create();
	ResultGPUTimerNode Renderer::m_GPUTimerRoot{};

	void Renderer::Init()
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->Init();

		s_RenderData.pViewUniformBuffer = UniformBuffer::Create(sizeof(ViewUniformData), UniformBufferBindingPoint::View);
		s_RenderData.pObjectUniformBuffer = UniformBuffer::Create(sizeof(ObjectUniformData), UniformBufferBindingPoint::Object);
		s_RenderData.pLightUniformBuffer = UniformBuffer::Create(sizeof(LightUniformData), UniformBufferBindingPoint::Light);
	}

	void Renderer::Update()
	{
		PROFILE_FUNCTION();

		RenderQueryProfiler::Instance().EndFrame();
		RenderQueryProfiler::Instance().PrepareQueryResult(m_GPUTimerRoot);

		s_RenderData.ViewUniformData.FrameCounter++;
		RenderQueryProfiler::Instance().BeginFrame(s_RenderData.ViewUniformData.FrameCounter);
	}

	void Renderer::Release()
	{
		PROFILE_FUNCTION();
	}

	void Renderer::SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->SetViewport(x_start, y_start, width, height);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->SetClearColor(color);
	}

	void Renderer::Clear()
	{
		PROFILE_FUNCTION();

		m_pRenderAPI->Clear();
	}

	/* 绘制一个视图 */
	void Renderer::RenderAView(RenderView* view)
	{
		PROFILE_FUNCTION();

		/* Fill view uniform buffer */
		s_RenderData.ViewUniformData.ViewMatrix = view->GetCullingCamera()->GetViewMatrix();
		s_RenderData.ViewUniformData.ProjectionMatrix = view->GetCullingCamera()->GetProjectionMatrix();
		s_RenderData.ViewUniformData.ViewProjectionMatrix = view->GetCullingCamera()->GetViewProjectionMatrix();
		s_RenderData.ViewUniformData.ViewPos = view->GetCullingCamera()->GetPosition();
		s_RenderData.pViewUniformBuffer->SetData(&s_RenderData.ViewUniformData, sizeof(ViewUniformData));

		view->Execute();
	}

	void Renderer::Submit(const SharedPtr<Material>& material, const MeshPrimitive& mesh_primitive, uint32_t index_count)
	{
		PROFILE_FUNCTION();

		material->Bind();
		mesh_primitive.VertexArray->Bind();

		m_pRenderAPI->ApplyRasterState(material->GetRasterState());

		if (mesh_primitive.VertexArray->GetIndexBuffer())
			m_pRenderAPI->DrawIndexed(mesh_primitive.PrimitiveType, mesh_primitive.VertexArray, index_count);
		else
			m_pRenderAPI->DrawArrays(mesh_primitive.PrimitiveType, mesh_primitive.VertexArray);

		material->Unbind();
		mesh_primitive.VertexArray->Unbind();
	}

	SharedPtr<VertexArray> Renderer::GetFullScreenVertexArray()
	{
		PROFILE_FUNCTION();

		/* Vertices */
		static constexpr float vertices[3 * 4] = 
		{
			-1.0f, -1.0f, 1.0f, 1.0f,
			 3.0f, -1.0f, 1.0f, 1.0f,
			-1.0f,  3.0f, 1.0f, 1.0f,
		};
		static const VertexBufferLayout vertex_buffer_layout = 
		{
			{ "a_Position", BufferDataType::Float4 }
		};
		const auto vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
		vertex_buffer->SetLayout(vertex_buffer_layout);

		/* Indices */
		static constexpr uint32_t indices[3] = 
		{
			0, 1, 2
		};
		const auto index_buffer = IndexBuffer::Create(indices, 3);

		auto vertex_array = VertexArray::Create();
		vertex_array->AddVertexBuffer(vertex_buffer);
		vertex_array->SetIndexBuffer(index_buffer);
		return vertex_array;
	}

	void Renderer::FillObjectUniformBuffer(const VisibleMeshObject& mesh_object)
	{
		static ObjectUniformData data;
		data.LocalToWorldMat = mesh_object.Local2WorldMat;
		data.ObjectId = mesh_object.ObjectId;
		s_RenderData.pObjectUniformBuffer->SetData(&data, sizeof(ObjectUniformData));
	}

	void Renderer::FillLightUniformBuffer(const ValidLight& valid_light)
	{
		static LightUniformData data;
		data.ColorIntensity = valid_light.ColorIntensity;
		data.LightDir = valid_light.LightDir;
		data.LightPos = valid_light.LightPos;
		data.LightType = valid_light.LightType;
		s_RenderData.pLightUniformBuffer->SetData(&data, sizeof(LightUniformData));
	}
}
