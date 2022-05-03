#include "Pch.h"
#include "EditorAssetManager.h"
#include <Kernel.h>

namespace Wuya
{
	EditorAssetManager* EditorAssetManager::s_pInstance = nullptr;

	EditorAssetManager* EditorAssetManager::Instance()
	{
		if (!s_pInstance)
			s_pInstance = new EditorAssetManager;

		return s_pInstance;
	}

	SharedPtr<Texture2D> EditorAssetManager::GetOrCreateTexture(const std::string& path)
	{
		PROFILE_FUNCTION();

		/* ����map�в��� */
		const auto it = m_TextureAssetMap.find(path);
		if (it != m_TextureAssetMap.end())
			return it->second;

		/* �Ҳ������򴴽� */
		auto texture = Texture2D::Create(path);
		m_TextureAssetMap.insert({ path, texture });
		return texture;
	}
}