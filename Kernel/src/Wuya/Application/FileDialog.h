#pragma once
namespace Wuya
{
	/* ϵͳ�ļ����� */
	class FileDialog
	{
	public:
		/* ͨ���ļ�����ѡȡָ�������ļ�·�� */
		static std::string OpenFile(const char* filter = "*.*");
		/* �����ļ���ָ��·�� */
		static std::string SaveFile(const char* filter = "*.*");
	};
}
