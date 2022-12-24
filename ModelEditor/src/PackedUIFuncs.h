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
		/* ����һ�����϶���Int UI */
		void DrawDragIntUI(const char* label, int& value, float label_width = 100.0f);
		/* ����һ�����϶���Float UI */
		void DrawDragFloatUI(const char* label, float& value, float label_width = 100.0f);
		/* ����һ�����϶���Float2 UI */
		void DrawDragFloat2UI(const char* label, glm::vec2& value, float label_width = 100.0f);
		/* ����һ�����϶���Float3 UI */
		void DrawDragFloat3UI(const char* label, glm::vec3& value, float label_width = 100.0f);
		/* ����һ�����϶���Float4 UI */
		void DrawDragFloat4UI(const char* label, glm::vec4& value, float label_width = 100.0f);
		/* ����һ��Combo */
		void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback = [](int) {}, float label_width = 100.0f);
	}
}
