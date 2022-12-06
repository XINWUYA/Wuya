#pragma once
#include <string>
#include <unordered_map>

namespace Wuya
{
	class Texture;
	struct TextureLoadConfig;

	/* 统一管理纹理资产, 避免重复创建 */
	class TextureAssetManager
	{
	public:
		~TextureAssetManager();

		/* 单例 */
		static TextureAssetManager& Instance();

		/* 获取Texture */
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config);

	private:
		TextureAssetManager() = default;

		/* 相对路径的Hash值作为Key */
		std::unordered_map<uint32_t, SharedPtr<Texture>> m_TextureAssetMap;
	};
}
