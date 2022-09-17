#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/**
	 * \brief 材质类
	 */
	class Material
	{
	public:
		Material() = default;
		~Material();

		/* 获取材质路径 */
		const std::string& GetPath() const { return m_Path; }
		/* 设置Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* 设置参数 */
		void SetParameters(const std::string& name, const std::any& param);
		/* 设置纹理 */
		void SetTexture(const SharedPtr<Texture>& texture, uint32_t slot);
		/* 设置光栅化状态 */
		void SetRasterState(RenderRasterState state) { m_RasterState = state; }
		RenderRasterState GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* 绑定材质中的各参数 */
		void Bind();
		/* 解绑材质 */
		void Unbind();

		/* 默认材质 */
		static SharedPtr<Material> Default();
		/* 错误材质 */
		static SharedPtr<Material> Error();

		/* 序列化和反序列化材质 */
		void Serializer();
		bool Deserializer(const std::string& path);

	private:
		/* 材质路径 */
		std::string m_Path{};
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* 材质所需的各种参数<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* 材质所需的各种纹理<Texture，Slot> */
		std::unordered_map<SharedPtr<Texture>, uint32_t> m_Textures{};
		/* 光栅化状态配置 */
		RenderRasterState m_RasterState{};
	};
}
