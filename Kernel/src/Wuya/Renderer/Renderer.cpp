#include "pch.h"
#include "Renderer.h"

namespace Wuya
{
	UniquePtr<RenderAPI> Renderer::m_pRenderAPI = RenderAPI::Create();

	void Renderer::Init()
	{
		m_pRenderAPI->Init();
	}

	void Renderer::Update()
	{
	}

	void Renderer::SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height)
	{
		m_pRenderAPI->SetViewport(x_start, y_start, width, height);
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		m_pRenderAPI->SetClearColor(color);
	}

	void Renderer::Clear()
	{
		m_pRenderAPI->Clear();
	}

	void Renderer::Submit(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count)
	{
		shader->Bind();
		vertex_array->Bind();

		if (vertex_array->GetIndexBuffer())
			m_pRenderAPI->DrawIndexed(vertex_array, index_count);
		else
			m_pRenderAPI->DrawArrays(vertex_array);
	}
}
