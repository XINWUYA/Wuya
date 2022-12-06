#include "Pch.h"
#include "AssetManager.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	TextureAssetManager::~TextureAssetManager()
	{
		m_TextureAssetMap.clear();
	}

	TextureAssetManager& TextureAssetManager::Instance()
	{
		static TextureAssetManager instance;

		return instance;
	}

	SharedPtr<Texture> TextureAssetManager::GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config)
	{
		PROFILE_FUNCTION();

		/* ����map�в��� */
		const auto it = m_TextureAssetMap.find(ToID(path));
		if (it != m_TextureAssetMap.end())
			return it->second;

		/* �Ҳ������򴴽� */
		auto texture = Texture::Create(path, load_config);
		m_TextureAssetMap.insert({ ToID(path), texture });
		return texture;
	}
}
