#pragma once
#include <string>
#include <unordered_map>

namespace Wuya
{
	class Texture;
	struct TextureLoadConfig;

	/* ͳһ���������ʲ�, �����ظ����� */
	class TextureAssetManager
	{
	public:
		~TextureAssetManager();

		/* ���� */
		static TextureAssetManager& Instance();

		/* ��ȡTexture */
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config);

	private:
		TextureAssetManager() = default;

		/* ���·����Hashֵ��ΪKey */
		std::unordered_map<uint32_t, SharedPtr<Texture>> m_TextureAssetMap;
	};
}
