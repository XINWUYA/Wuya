#pragma once
#include <Wuya.h>

/* 设置半透明UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* 背景透明Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()


/* 绘制自定义格式的UI */
class EditorUIFunctions
{
public:
	/* 绘制一个可拖动的Float UI */
	static void DrawDragFloatUI(const std::string& label, float& value, float label_width = 100.0f);
	/* 绘制一个常规vec3 UI */
	static void DrawVec3UI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* 绘制一个vec3 UI， 带XYZ */
	static void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* 绘制一个Color UI */
	static void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
	/* 绘制一个图片UI */
	static void DrawTextureUI(const std::string& label, Wuya::SharedPtr<Wuya::Texture2D>& texture, float& tiling_factor, float label_width = 100.0f);
	/* 绘制带选中的图像按钮UI */
	static void DrawCheckedImageButtonUI(const std::string& label, const Wuya::SharedPtr<Wuya::Texture2D>& texture, const ImVec2& size, bool checked = false, const std::function<void()>& button_func = []() {});
	/* 绘制一个Checkbox */
	static void DrawCheckboxUI(const std::string& label, bool& value, float label_width = 100.0f);
	/* 绘制一个Combo */
	static void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback = [](int) {}, float label_width = 100.0f);
	/* 绘制一个询问弹窗UI(todo: 还不能用，需要调试) */
	static bool DrawModalUI(const std::string& label, const std::string& content_text, bool& never_ask);

	/* todo: 以下方法无法稳定使用 */
	/* 注册显示Popup UI */
	static void RegisterPopupFunc(const std::string& label, std::function<void()> func);
	/* 显示PopupUI */
	static void DrawPopups();

private:
	/* 收集弹窗回调 */
	static std::unordered_map<std::string, std::function<void()>> m_PopupUIRegistry;
};

/* 将浮点数转换成指定精度的字符串 */
std::string ConvertFloat2String(float value, int precision = -1);
/* 根据指定变换恢复出平移、旋转和缩放 */
bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);