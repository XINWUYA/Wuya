#pragma once
#include <imgui.h>

namespace Wuya
{
/* ���ð�͸��UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* ����͸��Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()

class Texture;

/* ��չһЩ���õ�UI */
namespace ImGuiExt
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
	void DrawDragFloatUI(const std::string& label, float& value, float label_width = 100.0f);
	/* ����һ�����϶���Float2 UI */
	void DrawDragFloat2UI(const char* label, glm::vec2& value, float label_width = 100.0f);
	/* ����һ�����϶���Float3 UI */
	void DrawDragFloat3UI(const char* label, glm::vec3& value, float label_width = 100.0f);
	/* ����һ�����϶���Float4 UI */
	void DrawDragFloat4UI(const char* label, glm::vec4& value, float label_width = 100.0f);
	/* ����һ��vec3 UI�� ��XYZ */
	void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* ���ƴ�ѡ�е�ͼ��ťUI */
	void DrawCheckedImageButtonUI(const std::string& label, const SharedPtr<Texture>& texture, const ImVec2& size, bool checked = false, const std::function<void()>& button_func = []() {});
	/* ����һ��Checkbox */
	void DrawCheckboxUI(const std::string& label, bool& value, float label_width = 100.0f);
	/* ����һ��Combo */
	void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback = [](int) {}, float label_width = 100.0f);
	/* ����һ��ѯ�ʵ���UI(todo: �������ã���Ҫ����) */
	bool DrawModalUI(const std::string& label, const std::string& content_text, bool& never_ask);
}

}