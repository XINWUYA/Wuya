#pragma once
#include <functional>

#ifndef CREATE_UI
#define CREATE_UI(creator, name, data) EditorUICreator::Instance().CreateUI(creator, name, data)
#endif

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

