#pragma once
#include <Wuya.h>

/* ���ð�͸��UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* ����͸��Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()


/* �����Զ����ʽ��UI */
class EditorUIFunctions
{
public:
	/* ����һ�����϶���Float UI */
	static void DrawDragFloatUI(const std::string& label, float& value, float label_width = 100.0f);
	/* ����һ������vec3 UI */
	static void DrawVec3UI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* ����һ��vec3 UI�� ��XYZ */
	static void DrawVec3ControlUI(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float label_width = 100.0f);
	/* ����һ��Color UI */
	static void DrawColorUI(const std::string& label, glm::vec4& color, float label_width = 100.0f);
	/* ����һ��ͼƬUI */
	static void DrawTextureUI(const std::string& label, Wuya::SharedPtr<Wuya::Texture2D>& texture, float& tiling_factor, float label_width = 100.0f);
	/* ���ƴ�ѡ�е�ͼ��ťUI */
	static void DrawCheckedImageButtonUI(const std::string& label, const Wuya::SharedPtr<Wuya::Texture2D>& texture, const ImVec2& size, bool checked = false, const std::function<void()>& button_func = []() {});
	/* ����һ��Checkbox */
	static void DrawCheckboxUI(const std::string& label, bool& value, float label_width = 100.0f);
	/* ����һ��Combo */
	static void DrawComboUI(const std::string& label, const std::vector<std::string>& options, int& selected_idx, const std::function<void(int)>& callback = [](int) {}, float label_width = 100.0f);
	/* ����һ��ѯ�ʵ���UI(todo: �������ã���Ҫ����) */
	static bool DrawModalUI(const std::string& label, const std::string& content_text, bool& never_ask);

	/* todo: ���·����޷��ȶ�ʹ�� */
	/* ע����ʾPopup UI */
	static void RegisterPopupFunc(const std::string& label, std::function<void()> func);
	/* ��ʾPopupUI */
	static void DrawPopups();

private:
	/* �ռ������ص� */
	static std::unordered_map<std::string, std::function<void()>> m_PopupUIRegistry;
};

/* ��������ת����ָ�����ȵ��ַ��� */
std::string ConvertFloat2String(float value, int precision = -1);
/* ����ָ���任�ָ���ƽ�ơ���ת������ */
bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);