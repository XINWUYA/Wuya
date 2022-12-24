#pragma once
namespace Wuya
{
	class Texture;

	/* 封装好的部分常用UI */
	namespace PackedUIFuncs
	{
		/* 绘制一个普通的文本UI */
		void DrawCommonTextUI(const std::string& label, const std::string& value, float label_width = 100.0f);
		/* 绘制一个Color UI */
		void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
		/* 绘制一个图片UI */
		void DrawTextureUI(const std::string& label, SharedPtr<Texture>& texture, float& tiling_factor, float label_width = 100.0f);
		/* 绘制一个可拖动的Int UI */
		void DrawDragIntUI(const char* label, int& value, float label_width = 100.0f);
		/* 绘制一个可拖动的Float UI */
		void DrawDragFloatUI(const char* label, float& value, float label_width = 100.0f);
		/* 绘制一个可拖动的Float2 UI */
		void DrawDragFloat2UI(const char* label, glm::vec2& value, float label_width = 100.0f);
		/* 绘制一个可拖动的Float3 UI */
		void DrawDragFloat3UI(const char* label, glm::vec3& value, float label_width = 100.0f);
		/* 绘制一个可拖动的Float4 UI */
		void DrawDragFloat4UI(const char* label, glm::vec4& value, float label_width = 100.0f);
		/* 绘制一个Combo */
		void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback = [](int) {}, float label_width = 100.0f);
	}
}
