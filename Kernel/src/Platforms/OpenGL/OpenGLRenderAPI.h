#pragma once
#include <Wuya/Renderer/RenderAPI.h>

namespace Wuya
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		void Init() override;
		void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) override;
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;
	};
}
