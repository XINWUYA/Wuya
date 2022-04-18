#pragma once
#include <Wuya.h>

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
		std::string FileName;
		std::string FilePath;
		std::string FileType;
		float		FileSize;
		int			Depth;
		std::vector<Wuya::SharedPtr<FileNode>> ChildNodes;
		Wuya::SharedPtr<FileNode> ParentNode;

		FileNode() = default;
		FileNode(std::string name, std::string path, std::string type, const float size, const int depth)
			: FileName(std::move(name)), FilePath(std::move(path)), FileType(std::move(type)), FileSize(size), Depth(depth)
		{}

		bool operator==(const FileNode& other) const
		{
			return	this->FileName == other.FileName &&
					this->FilePath == other.FilePath &&
					this->FileType == other.FileType &&
					this->Depth    == other.Depth &&
					this->ParentNode == other.ParentNode;
		}
	};

	/* 递归生成文件节点树 */
	void BuildFileNodeTree(const Wuya::SharedPtr<FileNode>& parent_node);
	/* 生成文件节点UI(详细信息) */
	void BuildFileUIListTreeDetail(const Wuya::SharedPtr<FileNode>& node);
	/* 生成文件节点UI(简单索引) */
	void BuildFileUIListTreeSimple(const Wuya::SharedPtr<FileNode>& node);

	/* 获取文件相对路径 */
	static std::filesystem::path GetRelativePath(const std::filesystem::path& dir, const std::filesystem::path& path);

	/* 资源目录中文件列表节点 */
	Wuya::SharedPtr<FileNode> m_RootFileNodeTree{};
	/* 当前选中的文件节点 */
	Wuya::SharedPtr<FileNode> m_CurrentFileNode{};

	/* 只有目录文件被更改时，才更新文件节点树 */
	bool m_IsDirty{ true };

	/* 图标 */
	Wuya::SharedPtr<Wuya::Texture2D> m_pFolderIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFileIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFilterIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pSettingIcon;
};

