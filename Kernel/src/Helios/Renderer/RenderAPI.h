#pragma once
#include <glm/glm.hpp>
#include "RenderCommon.h"
#include "VertexArray.h"

namespace Helios
{
	class RenderAPI
	{
	public:
		enum : int
		{
			None = 0,
			OpenGL = 1,
			Metal = 2,
		};

		virtual ~RenderAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) = 0;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		/* 应用光栅化状态 */
		virtual void ApplyRasterState(RenderRasterState raster_state) = 0;
		/* 绘制调用 */
		virtual void DrawIndexed(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0, uint32_t index_offset = 0) = 0;
		virtual void DrawArrays(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array) = 0;
		/* Flush */
		virtual void Flush() = 0;

		/* GroupMarker */
		virtual void PushDebugGroup(const char* name) = 0;
		virtual void PopDebugGroup() = 0;

		/* 使用平台 */
		static int GetAPI() { return m_API; }
		/* 创建当前API */
		static SharedPtr<RenderAPI> Create();

	private:
		static int m_API;
	};
}


