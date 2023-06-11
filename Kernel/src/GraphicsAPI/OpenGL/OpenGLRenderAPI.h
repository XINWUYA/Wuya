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

		/* Ӧ�ù�դ������ */
		void ApplyRasterState(RenderRasterState raster_state) override;

		/* ���û������� */
		void DrawIndexed(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count = 0) override;
		void DrawArrays(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array) override;

		void Flush() override;

		/* ���DebugGroup���Ա���RenderDoc��ץ֡ʱʹPipeline�ṹ������ */
		void PushDebugGroup(const char* name) override;
		void PopDebugGroup() override;

	private:
		/* ��ȡOpenGL֧�ֵ���չ */
		void InitOpenGLExtensions();

		/* ��չ֧����� */
		struct
		{
			bool KHR_debug{ false };
			bool EXT_shader_framebuffer_fetch{ false };
			bool EXT_texture_filter_anisotropic{ false };
			bool EXT_texture_sRGB{ false };
		} m_SupportedExtensions;
	};
}
