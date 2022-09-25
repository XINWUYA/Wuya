#pragma once
#include <string>
#include <unordered_map>
#include <Wuya/Common/Common.h>

namespace Wuya
{
	class Texture;

	/* ͳһ����༭���ʲ�, �����ظ����� */
	class EditorAssetManager
	{
	public:
		~EditorAssetManager();

		/* ���� */
		static EditorAssetManager& Instance();

		/* ��ȡTexture */
		SharedPtr<Texture> GetOrCreateTexture(const std::string& path);

	private:
		EditorAssetManager() = default;
		std::unordered_map<std::string, SharedPtr<Texture>> m_TextureAssetMap; /* �ĳ�ʹ�����·����Hashֵ��ΪKey */
	};
}
