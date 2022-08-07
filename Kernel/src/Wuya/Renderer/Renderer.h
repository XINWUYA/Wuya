#pragma once
#include "RenderAPI.h"

namespace Wuya
{
	class RenderView;
	class Material;

	class Renderer
	{
	public:
		static void Init();
		static void Update();

		static void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		/* 绘制一个视图 */
		static void RenderAView(const SharedPtr<RenderView>& view);

		static void Submit(const SharedPtr<Material>& material, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0);

		static int CurrentAPI() { return RenderAPI::GetAPI(); }

		static const SharedPtr<RenderAPI>& GetRenderAPI() { return m_pRenderAPI; }

		static SharedPtr<VertexArray> GetFullScreenVertexArray();

	private:

		static SharedPtr<RenderAPI> m_pRenderAPI;
		/* 当前帧数计数 */
		uint32_t m_FrameCounter{ 0 };
	};
}
