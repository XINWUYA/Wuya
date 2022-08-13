#include "Pch.h"
#include "Resource.h"

namespace Wuya
{
	IResource::IResource(std::string name)
		: m_Name(std::move(name))
	{
	}

	IResource::IResource(std::string name, const SharedPtr<IResource>& parent)
		: m_Name(std::move(name)), m_Parent(parent)
	{
	}

	void IResource::NeedByPass(const SharedPtr<RenderPassNode>& pass_node)
	{
		if (!m_FirstPassNode)
		{
			m_FirstPassNode = m_LastPassNode = pass_node;
		}
		else
		{
			m_LastPassNode = pass_node;
		}

		m_RefCount++;

		/* ����Դ��ǰ�ýڵ㣬���PassҲ��Ҫ������ǰ�ýڵ� */
		if (m_Parent)
			m_Parent->NeedByPass(pass_node);
	}
}
