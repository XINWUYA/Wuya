#pragma once
#include <string>
#include <unordered_map>

namespace Wuya
{
	class Shader;
	class Texture;
	struct TextureLoadConfig;

	/* ͳһ����Shader, �����ظ����� */
	class ShaderAssetManager
	{
	public:
		/* ���� */
		static ShaderAssetManager& Instance();

		/* ���ļ��м���Shader */
		SharedPtr<Shader> GetOrLoad(const std::string& path);
		/* �������Shader */
		void Clear();

	private:
		/* Shader��Hash��Shader��ӳ��<NameHash, SharedPtr<Shader>> */
		std::unordered_map<uint32_t, SharedPtr<Shader>> m_ShaderAssetMap;
	};


	/* ͳһ���������ʲ�, �����ظ����� */
	class TextureAssetManager
	{
	public:
		/* ���� */
		static TextureAssetManager& Instance();

		/* ��ȡTexture */
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path);
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config);
		/* �������Texture */
		void Clear();

	private:
		TextureAssetManager() = default;

		/* ���·����Hashֵ��ΪKey */
		std::unordered_map<uint32_t, SharedPtr<Texture>> m_TextureAssetMap;
	};
}
