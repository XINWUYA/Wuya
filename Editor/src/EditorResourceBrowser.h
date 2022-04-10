#pragma once
#include <filesystem>
#include "Wuya/Common/Common.h"
#include "Wuya/Renderer/Texture.h"

class EditorResourceBrowser
{
public:
	EditorResourceBrowser();
	~EditorResourceBrowser() = default;

	void OnImGuiRenderer();

private:
	std::filesystem::path m_CurrentDirectory{};
	// Icons
	Wuya::SharedPtr<Wuya::Texture2D> m_pDirectoryIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFileIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFilterIcon;
};

