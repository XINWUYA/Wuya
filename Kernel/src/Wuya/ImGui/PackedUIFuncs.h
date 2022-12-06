#pragma once
namespace Wuya
{
	class Texture;

	/* ��װ�õĲ��ֳ���UI */
	namespace PackedUIFuncs
	{
		/* ����һ����ͨ���ı�UI */
		void DrawCommonTextUI(const std::string& label, const std::string& value, float label_width = 100.0f);
		/* ����һ��Color UI */
		void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
		/* ����һ��ͼƬUI */
		void DrawTextureUI(const std::string& label, SharedPtr<Texture>& texture, float& tiling_factor, float label_width = 100.0f);
	}
}
