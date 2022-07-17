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

		/* ��ȡ����·�� */
		const std::string& GetPath() const { return m_Path; }
		/* ����Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* ���ò��� */
		void SetParameters(const std::string& name, const std::any& param);
		/* �������� */
		void SetTexture(const SharedPtr<ITexture>& texture, uint32_t slot);
		/* �󶨲����еĸ����� */
		void Bind();

	private:
		/* ����·�� */
		std::string m_Path{};
		/* ��ɫ��ͼ */
		SharedPtr<ITexture> m_pAlbedoTexture{ nullptr };
		/* ������ͼ */
		SharedPtr<ITexture> m_pNormalTexture{ nullptr };
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* ��������ĸ��ֲ���<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* ��������ĸ�������<Texture��Slot> */
		std::unordered_map<SharedPtr<ITexture>, uint32_t> m_Textures{};
	};
}
