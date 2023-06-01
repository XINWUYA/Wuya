#include "Pch.h"
#include "AssetManager.h"
#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	/* ���� */
	ShaderAssetManager& ShaderAssetManager::Instance()
	{
		static ShaderAssetManager instance;
		return instance;
	}

	/* ���ļ��м���Shader */
	SharedPtr<Shader> ShaderAssetManager::GetOrLoad(const std::string& path)
	{
		PROFILE_FUNCTION();

		/* ����map�в��� */
		const auto key = ToID(path);
		const auto iter = m_ShaderAssetMap.find(key);
		if (iter != m_ShaderAssetMap.end())
			return iter->second;
		
		/* �Ҳ������򴴽� */
		auto shader = Shader::Create(path);
		m_ShaderAssetMap[key] = shader;
		return shader;
	}

	/* �������Shader */
	void ShaderAssetManager::Clear()
	{
		m_ShaderAssetMap.clear();
	}

	/* ���� */
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

		/* ����map�в��� */
		const auto key = ToID(path);
		const auto iter = m_TextureAssetMap.find(key);
		if (iter != m_TextureAssetMap.end())
		{
			/* �ж�LoadConfig�Ƿ��޸� */
			if (load_config == iter->second->GetTextureLoadConfig())
				return iter->second;
		}

		/* �Ҳ������򴴽� */
		auto texture = Texture::Create(path, load_config);
		m_TextureAssetMap[key] = texture;
		return texture;
	}

	/* �������Texture */
	void TextureAssetManager::Clear()
	{
		m_TextureAssetMap.clear();
	}
}
