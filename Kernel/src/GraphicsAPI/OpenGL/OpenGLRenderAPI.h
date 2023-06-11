#pragma once
#include "Wuya/Renderer/RenderAPI.h"

namespace Wuya
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		~OpenGLRenderAPI() override = default;

		void Init() override;
		void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) override;

		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		/* 应用光栅化参数 */
		void ApplyRasterState(RenderRasterState raster_state) override;

		/* 调用绘制命令 */
		void DrawIndexed(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0) override;
		void DrawArrays(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array) override;

		void Flush() override;

		/* 添加DebugGroup，以便在RenderDoc等抓帧时使Pipeline结构更清晰 */
		void PushDebugGroup(const char* name) override;
		void PopDebugGroup() override;

	private:
		/* 获取OpenGL支持的扩展 */
		void InitOpenGLExtensions();

		/* 扩展支持情况 */
		struct
		{
			bool KHR_debug{ false };
			bool EXT_shader_framebuffer_fetch{ false };
			bool EXT_texture_filter_anisotropic{ false };
			bool EXT_texture_sRGB{ false };
		} m_SupportedExtensions;
	};
}
