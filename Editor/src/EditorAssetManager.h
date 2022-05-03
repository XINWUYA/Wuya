#pragma once
#include <string>
#include <unordered_map>
#include <Wuya/Common/Common.h>

namespace Wuya
{
	class Texture2D;

	/* 统一管理编辑器资产, 避免重复创建 */
	class EditorAssetManager
	{
	public:
		/* 单例 */
		static EditorAssetManager* Instance();

		/* 获取Texture */
		SharedPtr<Texture2D> GetOrCreateTexture(const std::string& path);

	private:
		EditorAssetManager() = default;
		std::unordered_map<std::string, SharedPtr<Texture2D>> m_TextureAssetMap; /* 改成使用相对路径的Hash值作为Key */
		static EditorAssetManager* s_pInstance;
	};
}
