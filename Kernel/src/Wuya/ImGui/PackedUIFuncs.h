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
	}
}
