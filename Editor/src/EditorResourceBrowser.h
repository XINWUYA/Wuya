#pragma once
#include <Wuya.h>

/* ��Դ�������ࣺ�༭����Դ���� */
class EditorResourceBrowser
{
public:
	EditorResourceBrowser();
	~EditorResourceBrowser() = default;

	/* �������UI */
	void OnImGuiRenderer();

private:
	/* ��ԴĿ¼�µ��ļ��ڵ㣬�����ļ��к��ļ� */
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

	/* �ݹ������ļ��ڵ��� */
	void BuildFileNodeTree(const Wuya::SharedPtr<FileNode>& parent_node);
	/* �����ļ��ڵ�UI(��ϸ��Ϣ) */
	void BuildFileUIListTreeDetail(const Wuya::SharedPtr<FileNode>& node);
	/* �����ļ��ڵ�UI(������) */
	void BuildFileUIListTreeSimple(const Wuya::SharedPtr<FileNode>& node);

	/* ��ȡ�ļ����·�� */
	static std::filesystem::path GetRelativePath(const std::filesystem::path& dir, const std::filesystem::path& path);

	/* ��ԴĿ¼���ļ��б�ڵ� */
	Wuya::SharedPtr<FileNode> m_RootFileNodeTree{};
	/* ��ǰѡ�е��ļ��ڵ� */
	Wuya::SharedPtr<FileNode> m_CurrentFileNode{};

	/* ֻ��Ŀ¼�ļ�������ʱ���Ÿ����ļ��ڵ��� */
	bool m_IsDirty{ true };

	/* ͼ�� */
	Wuya::SharedPtr<Wuya::Texture2D> m_pFolderIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFileIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pFilterIcon;
	Wuya::SharedPtr<Wuya::Texture2D> m_pSettingIcon;
};

