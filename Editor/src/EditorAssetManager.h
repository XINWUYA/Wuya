#pragma once
#include <string>
#include <unordered_map>
#include <Wuya/Common/Common.h>

namespace Wuya
{
	class Texture2D;

	/* ͳһ����༭���ʲ�, �����ظ����� */
	class EditorAssetManager
	{
	public:
		/* ���� */
		static EditorAssetManager* Instance();

		/* ��ȡTexture */
		SharedPtr<Texture2D> GetOrCreateTexture(const std::string& path);

	private:
		EditorAssetManager() = default;
		std::unordered_map<std::string, SharedPtr<Texture2D>> m_TextureAssetMap; /* �ĳ�ʹ�����·����Hashֵ��ΪKey */
		static EditorAssetManager* s_pInstance;
	};
}
