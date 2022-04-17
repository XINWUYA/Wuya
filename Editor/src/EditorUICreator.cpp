#include "EditorUICreator.h"
#include "Wuya.h"
#include <imgui.h>

EditorUICreator& EditorUICreator::Instance()
{
	static EditorUICreator instance;
	return instance;
}

void EditorUICreator::RegisterUICreator(const std::string& creator_tag, UICreatorFunc func)
{
	m_UICreators[creator_tag] = std::move(func);
}

void EditorUICreator::CreateUI(const std::string& creator_tag, const std::string& name, void* data)
{
	if (m_UICreators.find(creator_tag) != m_UICreators.end())
	{
		m_UICreators[creator_tag](name, data);
	}
	else
		EDITOR_LOG_ERROR("Unknown UI creator£º {}.", creator_tag);
}

EditorUICreator::EditorUICreator()
{
	// String
	m_UICreators["String"] = [](const std::string& name, void* data)
	{
		const std::string label = "##" + name;
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Text((*static_cast<std::string*>(data)).c_str());
	};
}
