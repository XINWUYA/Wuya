#include "Pch.h"
#include "AssetManager.h"
#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	/* 单例 */
	ShaderAssetManager& ShaderAssetManager::Instance()
	{
		static ShaderAssetManager instance;
		return instance;
	}

	/* 从文件中加载Shader */
	SharedPtr<Shader> ShaderAssetManager::GetOrLoad(const std::string& path)
	{
		PROFILE_FUNCTION();

		/* 先在map中查找 */
		const auto key = ToID(path);
		const auto iter = m_ShaderAssetMap.find(key);
		if (iter != m_ShaderAssetMap.end())
			return iter->second;
		
		/* 找不到，则创建 */
		auto shader = Shader::Create(path);
		m_ShaderAssetMap[key] = shader;
		return shader;
	}

	/* 清空所有Shader */
	void ShaderAssetManager::Clear()
	{
		m_ShaderAssetMap.clear();
	}

	/* 单例 */
	TextureAssetManager& TextureAssetManager::Instance()
	{
		static TextureAssetManager instance;
		return instance;
	}

	SharedPtr<Texture> TextureAssetManager::GetOrCreateTexture(const std::string& path)
	{
		constexpr TextureLoadConfig load_config;
		return GetOrCreateTexture(path, load_config);
	}

	SharedPtr<Texture> TextureAssetManager::GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config)
	{
		PROFILE_FUNCTION();

		/* 先在map中查找 */
		const auto key = ToID(path);
		const auto iter = m_TextureAssetMap.find(key);
		if (iter != m_TextureAssetMap.end())
		{
			/* 判断LoadConfig是否被修改 */
			if (load_config == iter->second->GetTextureLoadConfig())
				return iter->second;
		}

		/* 找不到，则创建 */
		auto texture = Texture::Create(path, load_config);
		m_TextureAssetMap[key] = texture;
		return texture;
	}

	/* 清空所有Texture */
	void TextureAssetManager::Clear()
	{
		m_TextureAssetMap.clear();
	}
}
