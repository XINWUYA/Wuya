#pragma once
#include <any>

namespace Wuya
{
	class ITexture;
	class Shader;

	class Material
	{
	public:
		Material() = default;
		Material(const std::string& path);
		~Material() = default;

		/* 获取材质路径 */
		const std::string& GetPath() const { return m_Path; }
		/* 设置Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* 设置参数 */
		void SetParameters(const std::string& name, const std::any& param);
		/* 设置纹理 */
		void SetTexture(const SharedPtr<ITexture>& texture, uint32_t slot);
		/* 绑定材质中的各参数 */
		void Bind();

	private:
		/* 材质路径 */
		std::string m_Path{};
		/* 颜色贴图 */
		SharedPtr<ITexture> m_pAlbedoTexture{ nullptr };
		/* 法线贴图 */
		SharedPtr<ITexture> m_pNormalTexture{ nullptr };
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* 材质所需的各种参数<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* 材质所需的各种纹理<Texture，Slot> */
		std::unordered_map<SharedPtr<ITexture>, uint32_t> m_Textures{};
	};
}
