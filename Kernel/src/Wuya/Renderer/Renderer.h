#pragma once
#include "RenderAPI.h"
#include "RenderQuery.h"

namespace Wuya
{
	class RenderView;
	class Material;
	class UniformBuffer;
	struct VisibleMeshObject;
	struct ValidLight;
	struct MeshPrimitive;

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
		static void RenderAView(RenderView* view);

		static void Submit(const SharedPtr<Material>& material, const MeshPrimitive& mesh_primitive, uint32_t index_count = 0);

		static int CurrentAPI() { return RenderAPI::GetAPI(); }

		static const SharedPtr<RenderAPI>& GetRenderAPI() { return m_pRenderAPI; }

		static const ResultGPUTimerNode& GetGPUTimerRoot() { return m_GPUTimerRoot; }

		static SharedPtr<VertexArray> GetFullScreenVertexArray();

		static void FillObjectUniformBuffer(const VisibleMeshObject& mesh_object);

		static void FillLightUniformBuffer(const ValidLight& valid_light);

	private:
		static SharedPtr<RenderAPI> m_pRenderAPI;
		static ResultGPUTimerNode m_GPUTimerRoot;
	};
}
