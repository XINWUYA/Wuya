#pragma once
#include <string>
#include <unordered_map>

namespace Helios
{
	class DeviceShader;
	class DeviceTexture;
	struct TextureLoadConfig;

	/* 统一管理Shader, 避免重复创建 */
	class ShaderAssetManager
	{
	public:
		/* 单例 */
		static ShaderAssetManager& Instance();

		/* 从文件中加载Shader */
		SharedPtr<DeviceShader> GetOrLoad(const std::string& path);
		/* 清空所有Shader */
		void Clear();

	private:
		/* Shader名Hash到Shader的映射<NameHash, SharedPtr<DeviceShader>> */
		std::unordered_map<uint32_t, SharedPtr<DeviceShader>> m_ShaderAssetMap;
	};


	/* 统一管理纹理资产, 避免重复创建 */
	class TextureAssetManager
	{
	public:
		/* 单例 */
		static TextureAssetManager& Instance();

		/* 获取Texture */
		SharedPtr<DeviceTexture> GetOrCreateTexture(const std::string& path);
		SharedPtr<DeviceTexture> GetOrCreateTexture(const std::string& path, const TextureLoadConfig& load_config);
		/* 清空所有Texture */
		void Clear();

	private:
		TextureAssetManager() = default;

		/* 相对路径的Hash值作为Key */
		std::unordered_map<uint32_t, SharedPtr<DeviceTexture>> m_TextureAssetMap;
	};
}
