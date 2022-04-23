#include "pch.h"
#include "EditorAssetManager.h"
#include <Wuya.h>

EditorAssetManager* EditorAssetManager::s_pInstance = nullptr;

EditorAssetManager* EditorAssetManager::Instance()
{
	if (!s_pInstance)
		s_pInstance = new EditorAssetManager;

	return s_pInstance;
}

Wuya::SharedPtr<Wuya::Texture2D> EditorAssetManager::GetOrCreateTexture(const std::string& path)
{
	PROFILE_FUNCTION();

	/* ����map�в��� */
	const auto it = m_TextureAssetMap.find(path);
	if (it != m_TextureAssetMap.end())
		return it->second;

	/* �Ҳ������򴴽� */
	auto texture = Wuya::Texture2D::Create(path);
	m_TextureAssetMap.insert({ path, texture });
	return texture;
}
