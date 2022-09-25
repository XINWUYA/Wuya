#include "Pch.h"
#include "OpenGLRenderAPI.h"
#include "OpenGLCommon.h"
#include <unordered_set>
#include <glad/glad.h>

namespace Wuya
{
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         CORE_LOG_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       CORE_LOG_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          CORE_LOG_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: CORE_LOG_INFO(message); return;
		default:
			CORE_LOG_ERROR("Unknown severity level!");
		}
	}

	void OpenGLRenderAPI::Init()
	{
		PROFILE_FUNCTION();

#ifdef WUYA_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

		// Blend
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Depth
		glEnable(GL_DEPTH_TEST);

		InitOpenGLExtensions();
	}

	void OpenGLRenderAPI::SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height)
	{
		glViewport((GLint)x_start, (GLint)y_start, (GLsizei)width, (GLsizei)height);
	}

	void OpenGLRenderAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.b, color.b, color.a);
	}

	void OpenGLRenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/* 应用光栅化参数 */
	void OpenGLRenderAPI::ApplyRasterState(RenderRasterState raster_state)
	{
		PROFILE_FUNCTION();

		/* 设置CullMode */
		switch (raster_state.CullMode)
		{
		case CullMode::Cull_None:
			glDisable(GL_CULL_FACE);
			break;
		case CullMode::Cull_Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case CullMode::Cull_Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CullMode::Cull_Front_And_Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			break;
		}

		/* 设置正面判定方式 */
		switch (raster_state.FrontFaceType)
		{
		case FrontFaceType::CW:
			glFrontFace(GL_CW);
			break;
		case FrontFaceType::CCW:
			glFrontFace(GL_CCW);
			break;
		}

		/* 设置混合模式 */
		if (raster_state.EnableBlend)
		{
			glEnable(GL_BLEND);
			glBlendEquationSeparate(
				TranslateToOpenGLBlendEquation(raster_state.BlendEquationRGB),
				TranslateToOpenGLBlendEquation(raster_state.BlendEquationA));
			glBlendFuncSeparate(
				TranslateToOpenGLBlendFunc(raster_state.BlendFuncSrcRGB), 
				TranslateToOpenGLBlendFunc(raster_state.BlendFuncDstRGB), 
				TranslateToOpenGLBlendFunc(raster_state.BlendFuncSrcA), 
				TranslateToOpenGLBlendFunc(raster_state.BlendFuncDstA));
		}
		else
		{
			glDisable(GL_BLEND);
		}

		/* 设置深度测试 */
		if (raster_state.EnableDepthWrite)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(TranslateToOpenGLCompareFunc(raster_state.DepthCompareFunc));
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		/* 设置颜色Mask */
		glColorMask((GLboolean)raster_state.EnableColorWrite, (GLboolean)raster_state.EnableColorWrite, (GLboolean)raster_state.EnableColorWrite, (GLboolean)raster_state.EnableColorWrite);

	}

	void OpenGLRenderAPI::DrawIndexed(const SharedPtr<VertexArray>& vertex_array, uint32_t index_count)
	{
		PROFILE_FUNCTION();

		const uint32_t count = index_count ? index_count : vertex_array->GetVertexCount();
		glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::DrawArrays(const SharedPtr<VertexArray>& vertex_array)
	{
		PROFILE_FUNCTION();

		const auto count = vertex_array->GetVertexCount();
		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	void OpenGLRenderAPI::Flush()
	{
		PROFILE_FUNCTION();

		glFlush();
	}

	void OpenGLRenderAPI::PushDebugGroup(const char* name)
	{
		PROFILE_FUNCTION();

		if (m_SupportedExtensions.KHR_debug)
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
	}

	void OpenGLRenderAPI::PopDebugGroup()
	{
		PROFILE_FUNCTION();

		if (m_SupportedExtensions.KHR_debug)
			glPopDebugGroup();
	}

	/* 获取OpenGL支持的扩展 */
	void OpenGLRenderAPI::InitOpenGLExtensions()
	{
		PROFILE_FUNCTION();

		/* 获取扩展数 */
		GLint num = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &num);
		/* 收集扩展 */
		std::unordered_set<std::string_view> extensions;
		for (auto i = 0; i < num; ++i)
			extensions.emplace((const char*)glGetStringi(GL_EXTENSIONS, (GLuint)i));

#ifdef WUYA_DEBUG
		CORE_LOG_INFO("Supported Extensions:");
		for (auto& ext : extensions)
			CORE_LOG_INFO("- {}", ext);
#endif

		m_SupportedExtensions.KHR_debug = extensions.find(std::string_view("GL_KHR_debug")) != extensions.end();
		m_SupportedExtensions.EXT_shader_framebuffer_fetch = extensions.find(std::string_view("GL_EXT_shader_framebuffer_fetch")) != extensions.end();
		m_SupportedExtensions.EXT_texture_filter_anisotropic = extensions.find(std::string_view("GL_EXT_texture_filter_anisotropic")) != extensions.end();
		m_SupportedExtensions.EXT_texture_sRGB = extensions.find(std::string_view("GL_EXT_texture_sRGB")) != extensions.end();
	}
}
