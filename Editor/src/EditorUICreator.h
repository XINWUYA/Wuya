#pragma once
#include <functional>
#include <Wuya/Common/Common.h>

#ifndef CREATE_UI
#define CREATE_UI(creator, name, data) EditorUICreator::Instance().CreateUI(creator, name, data)
#endif

/* ÉèÖÃ°ëÍ¸Ã÷UI */
#define START_STYLE_ALPHA(alpha) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (alpha))
#define END_STYLE_ALPHA ImGui::PopStyleVar()

/* ±³¾°Í¸Ã÷Button */
#define START_TRANSPARENT_BUTTON ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0))
#define END_TRANSPARENT_BUTTON ImGui::PopStyleColor()

using UICreatorFunc = std::function<void(const std::string&, void*)>;
class EditorUICreator
{
public:
	static EditorUICreator& Instance();

	void RegisterUICreator(const std::string& creator_tag, UICreatorFunc func);
	void CreateUI(const std::string& creator_tag, const std::string& name, void* data);

private:
	EditorUICreator();
	~EditorUICreator() = default;

	std::unordered_map<std::string, UICreatorFunc> m_UICreators{};
};
