#pragma once
#include "Helios/Renderer/RenderAPI.h"

namespace Helios
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		~OpenGLRenderAPI() override = default;

		void Init() override;
		void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) override;
		void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		/* 应用光栅化状态 */
		void ApplyRasterState(RenderRasterState raster_state) override;

		/* 绘制调用 */
		void DrawIndexed(PrimitiveType type, const SharedPtr<DeviceVertexArray>& vertex_array, uint32_t index_count = 0, uint32_t index_offset = 0) override;
		void DrawArrays(PrimitiveType type, const SharedPtr<DeviceVertexArray>& vertex_array) override;

		void Flush() override;

		/* 压入DebugGroup，以便在RenderDoc抓帧时使用Pipeline结构组织 */
		void PushDebugGroup(const char* name) override;
        void PopDebugGroup() override;

	private:
		/* 获取OpenGL支持的扩展 */
		void InitOpenGLExtensions();

		/* 扩展支持标记 */
		struct
		{
			bool KHR_debug{ false };
			bool EXT_shader_framebuffer_fetch{ false };
			bool EXT_texture_filter_anisotropic{ false };
			bool EXT_texture_sRGB{ false };
		} m_SupportedExtensions;
	};
}
