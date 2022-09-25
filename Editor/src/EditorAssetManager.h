#pragma once
#include <string>
#include <unordered_map>
#include <Wuya/Common/Common.h>

namespace Wuya
{
	class Texture;

	/* 统一管理编辑器资产, 避免重复创建 */
	class EditorAssetManager
	{
	public:
		~EditorAssetManager();

		/* 单例 */
		static EditorAssetManager& Instance();

		/* 获取Texture */
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path);

	private:
		EditorAssetManager() = default;
		std::unordered_map<std::string, SharedPtr<Texture>> m_TextureAssetMap; /* 改成使用相对路径的Hash值作为Key */
	};
}
