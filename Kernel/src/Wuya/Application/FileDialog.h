#pragma once
namespace Wuya
{
	/* 系统文件弹窗 */
	class FileDialog
	{
	public:
		/* 通过文件窗口选取指定类型文件路径 */
		static std::string OpenFile(const char* filter = "*.*");
		/* 保存文件到指定路径 */
		static std::string SaveFile(const char* filter = "*.*");
	};
}
