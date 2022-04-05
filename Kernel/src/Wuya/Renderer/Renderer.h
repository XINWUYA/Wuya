#pragma once
#include "RenderAPI.h"
#include "Shader.h"

namespace Wuya
{
	class Renderer
	{
	public:
		static void Init();
		static void Update();

		static void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		static void Submit(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0);

		static int CurrentAPI() { return RenderAPI::GetAPI(); }

	private:
		static UniquePtr<RenderAPI> m_pRenderAPI;
	};
}
