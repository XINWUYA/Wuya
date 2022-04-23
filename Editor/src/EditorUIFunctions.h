#pragma once
#include <Wuya.h>

/* 设置半透明UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* 背景透明Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()


class EditorUIFunctions
{
public:
	/* 绘制一个常规vec3 UI */
	static void DrawVec3UI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* 绘制一个vec3 UI， 带XYZ */
	static void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* 绘制一个Color UI */
	static void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
	/* 绘制一个图片UI */
	static void DrawTextureUI(const std::string& label, Wuya::SharedPtr<Wuya::Texture2D>& texture, float& tiling_factor, float label_width = 100.0f);

};
