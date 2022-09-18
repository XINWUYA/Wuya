#pragma once
#include <any>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class Texture;
	class Shader;

	/**
	 * \brief ������
	 */
	class Material
	{
	public:
		Material() = default;
		~Material();

		/* ��ȡ����·�� */
		const std::string& GetPath() const { return m_Path; }
		/* ����Shader */
		void SetShader(const SharedPtr<Shader>& shader) { m_pShader = shader; }
		const SharedPtr<Shader>& GetShader() const { return m_pShader; }
		/* ���ò��� */
		void SetParameters(const std::string& name, const std::any& param);
		/* �������� */
		void SetTexture(const SharedPtr<Texture>& texture, uint32_t slot);
		/* ���ù�դ��״̬ */
		void SetRasterState(RenderRasterState state) { m_RasterState = state; }
		RenderRasterState GetRasterState() const { return m_RasterState; }
		RenderRasterState& GetRasterState() { return m_RasterState; }

		/* �󶨲����еĸ����� */
		void Bind();
		/* ������ */
		void Unbind();

		/* Ĭ�ϲ��� */
		static SharedPtr<Material> Default();
		/* ������� */
		static SharedPtr<Material> Error();

		/* ���л��ͷ����л����� */
		void Serializer();
		bool Deserializer(const std::string& path);

	private:
		/* ����·�� */
		std::string m_Path{};
		/* Shader */
		SharedPtr<Shader> m_pShader{ nullptr };
		/* ��������ĸ��ֲ���<Name, Vale> */
		std::unordered_map<std::string, std::any> m_Parameters{};
		/* ��������ĸ�������<Texture��Slot> */
		std::unordered_map<SharedPtr<Texture>, uint32_t> m_Textures{};
		/* ��դ��״̬���� */
		RenderRasterState m_RasterState{};
	};
}
