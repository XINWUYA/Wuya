#pragma once
#include "RenderAPI.h"

namespace Wuya
{
	class RenderView;
	class Material;
	class UniformBuffer;
	struct VisibleMeshObject;

	class Renderer
	{
	public:
		static void Init();
		static void Update();
		static void Release();

		static void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height);
		static void SetClearColor(const glm::vec4& color);
		static void Clear();

		/* 绘制一个视图 */
		static void RenderAView(const SharedPtr<RenderView>& view);

		static void Submit(const SharedPtr<Material>& material, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0);

		static int CurrentAPI() { return RenderAPI::GetAPI(); }

		static const SharedPtr<RenderAPI>& GetRenderAPI() { return m_pRenderAPI; }

		static SharedPtr<VertexArray> GetFullScreenVertexArray();

		static void FillObjectUniformBuffer(const VisibleMeshObject& mesh_object);

	private:
		static SharedPtr<RenderAPI> m_pRenderAPI;
	};
}
