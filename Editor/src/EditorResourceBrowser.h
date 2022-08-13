#pragma once
#include <Kernel.h>

namespace Wuya
{
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
			std::string FileName;							/* �ļ��� */
			std::string FilePath;							/* ��¼��ǰ�ļ�·�� */
			std::string FileType;							/* �ļ����ͣ�Folder/File */
			float		FileSize{ 0 };						/* �ļ���С */
			int			Depth{ -1 };						/* �ļ��о����Ŀ¼�Ĳ��� */
			std::vector<SharedPtr<FileNode>> ChildNodes;	/* �ӽڵ��ļ���Ŀ¼�¿����ж�� */
			WeakPtr<FileNode> ParentNode;					/* ���ڵ㣬����ʹ��WeakPtr, ��Ȼ���������ָ��ѭ�����ã������ڴ�й© */

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

		/* �ݹ������ļ��ڵ��� */
		void BuildFileNodeTree(const SharedPtr<FileNode>& parent_node);
		/* �����ļ��ڵ�UI(��ϸ��Ϣ) */
		void BuildFileUIListTreeDetail(const SharedPtr<FileNode>& node);
		/* �����ļ��ڵ�UI(������) */
		void BuildFileUIListTreeSimple(const SharedPtr<FileNode>& node);

		/* ��ȡ�ļ����·�� */
		static std::filesystem::path GetRelativePath(const std::filesystem::path& dir, const std::filesystem::path& path);

		/* ��ԴĿ¼���ļ��б�ڵ� */
		SharedPtr<FileNode> m_RootFileNodeTree{};
		/* ��ǰѡ�е��ļ��ڵ� */
		SharedPtr<FileNode> m_CurrentFileNode{};

		/* ֻ��Ŀ¼�ļ�������ʱ���Ÿ����ļ��ڵ��� */
		bool m_IsDirty{ true };

		/* ͼ�� */
		SharedPtr<Texture2D> m_pFolderIcon;
		SharedPtr<Texture2D> m_pFileIcon;
		SharedPtr<Texture2D> m_pFilterIcon;
		SharedPtr<Texture2D> m_pMenuIcon;
		SharedPtr<Texture2D> m_pReturnIcon;
	};
}
