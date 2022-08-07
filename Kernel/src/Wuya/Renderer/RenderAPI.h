#pragma once
#include <glm/glm.hpp>
#include "RenderCommon.h"
#include "VertexArray.h"

namespace Wuya
{
	class RenderAPI
	{
	public:
		enum : int
		{
			None = 0,
			OpenGL = 1,
		};

		virtual ~RenderAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		/* Ӧ�ù�դ������ */
		virtual void ApplyRasterState(RenderRasterState raster_state) = 0;

		/* ���û������� */
		virtual void DrawIndexed(const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0) = 0;
		virtual void DrawArrays(const SharedPtr<VertexArray>& vertex_array) = 0;

		/* Flush */
		virtual void Flush() = 0;

		/* GroupMarker */
		virtual void PushDebugGroup(const char* name) = 0;
		virtual void PopDebugGroup() = 0;

		static int GetAPI() { return m_API; }
		/* ������ǰAPI */
		static SharedPtr<RenderAPI> Create();

	private:
		static int m_API;
	};
}


