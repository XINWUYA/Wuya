#include "Pch.h"
#include "EditorAssetManager.h"

namespace Wuya
{
	EditorAssetManager::~EditorAssetManager()
	{
		m_TextureAssetMap.clear();
	}

	EditorAssetManager& EditorAssetManager::Instance()
	{
		static EditorAssetManager instance;

		return instance;
	}

	SharedPtr<Texture> EditorAssetManager::GetOrCreateTexture(const std::string& path)
	{
		PROFILE_FUNCTION();

		/* ����map�в��� */
		const auto it = m_TextureAssetMap.find(path);
		if (it != m_TextureAssetMap.end())
			return it->second;

		/* �Ҳ������򴴽� */
		auto texture = Texture::Create(path);
		m_TextureAssetMap.insert({ path, texture });
		return texture;
	}
}