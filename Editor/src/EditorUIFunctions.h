#pragma once
#include <Wuya.h>

/* ���ð�͸��UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* ����͸��Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()


class EditorUIFunctions
{
public:
	/* ����һ������vec3 UI */
	static void DrawVec3UI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* ����һ��vec3 UI�� ��XYZ */
	static void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* ����һ��Color UI */
	static void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
	/* ����һ��ͼƬUI */
	static void DrawTextureUI(const std::string& label, Wuya::SharedPtr<Wuya::Texture2D>& texture, float& tiling_factor, float label_width = 100.0f);

};
