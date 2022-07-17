#include "Pch.h"
#include "Resource.h"

namespace Wuya
{
	IResource::IResource(const std::string& name)
		: m_Name(name), m_Parent(this)
	{
	}

	IResource::IResource(const std::string& name, IResource* parent)
		: m_Name(name), m_Parent(parent)
	{
	}

	void IResource::NeedByPass(RenderPassNode* pass_node)
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

		/* 若资源有前置节点，则该Pass也需要依赖该前置节点 */
		if (m_Parent != this)
			m_Parent->NeedByPass(pass_node);
	}
}
