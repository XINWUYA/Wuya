#pragma once
#include <Kernel.h>

namespace Wuya
{
	/* 资源管理器类：编辑器资源窗口 */
	class EditorResourceBrowser
	{
	public:
		EditorResourceBrowser();
		~EditorResourceBrowser() = default;

		/* 绘制相关UI */
		void OnImGuiRenderer();

	private:
		/* 资源目录下的文件节点，包括文件夹和文件 */
		struct FileNode
		{
			std::string FileName;							/* 文件名 */
			std::string FilePath;							/* 记录当前文件路径 */
			std::string FileType;							/* 文件类型：Folder/File */
			float		FileSize{ 0 };						/* 文件大小 */
			int			Depth{ -1 };						/* 文件夹距离根目录的层数 */
			std::vector<SharedPtr<FileNode>> ChildNodes;	/* 子节点文件，目录下可能有多个 */
			WeakPtr<FileNode> ParentNode;					/* 父节点，必须使用WeakPtr, 不然会出现智能指针循环引用，导致内存泄漏 */

			FileNode() = default;
			FileNode(std::string name, std::string path, std::string type, const float size, const int depth)
				: FileName(std::move(name)), FilePath(std::move(path)), FileType(std::move(type)), FileSize(size), Depth(depth)
			{}
			~FileNode()
			{
				ChildNodes.clear();
			}

			bool operator==(const FileNode& other) const
			{
				return	this->FileName == other.FileName &&
					this->FilePath == other.FilePath &&
					this->FileType == other.FileType &&
					this->Depth == other.Depth &&
					this->ParentNode.lock() == other.ParentNode.lock();
			}
		};

		/* 递归生成文件节点树 */
		void BuildFileNodeTree(const SharedPtr<FileNode>& parent_node);
		/* 生成文件节点UI(详细信息) */
		void BuildFileUIListTreeDetail(const SharedPtr<FileNode>& node);
		/* 生成文件节点UI(简单索引) */
		void BuildFileUIListTreeSimple(const SharedPtr<FileNode>& node);

		/* 获取文件相对路径 */
		static std::filesystem::path GetRelativePath(const std::filesystem::path& dir, const std::filesystem::path& path);

		/* 资源目录中文件列表节点 */
		SharedPtr<FileNode> m_RootFileNodeTree{};
		/* 当前选中的文件节点 */
		SharedPtr<FileNode> m_CurrentFileNode{};

		/* 只有目录文件被更改时，才更新文件节点树 */
		bool m_IsDirty{ true };

		/* 图标 */
		SharedPtr<Texture2D> m_pFolderIcon;
		SharedPtr<Texture2D> m_pFileIcon;
		SharedPtr<Texture2D> m_pFilterIcon;
		SharedPtr<Texture2D> m_pMenuIcon;
		SharedPtr<Texture2D> m_pReturnIcon;
	};
}
