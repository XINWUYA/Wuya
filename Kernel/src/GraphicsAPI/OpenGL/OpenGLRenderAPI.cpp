#include "Pch.h"
#include "OpenGLRenderAPI.h"
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

	void OpenGLRenderAPI::DrawIndexed(const SharedPtr<VertexArray>& vertex_array, uint32_t index_count)
	{
		const uint32_t count = index_count ? index_count : vertex_array->GetVertexCount();
		glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderAPI::DrawArrays(const SharedPtr<VertexArray>& vertex_array)
	{
		const auto count = vertex_array->GetVertexCount();
		glDrawArrays(GL_TRIANGLES, 0, count);
	}
}
