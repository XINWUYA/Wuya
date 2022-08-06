#include "Pch.h"
#include "Blackboard.h"

namespace Wuya
{
	/* [] ���أ�������Դ��ֱ�Ӵ�ȡ */
	FrameGraphResourceHandle& Blackboard::operator[](const std::string& name) noexcept
	{
		auto [iter, _] = m_NameToResourceHandleMap.insert_or_assign(name, FrameGraphResourceHandle{});
		return iter->second;
	}

	/* ��ŵ�ӳ���� */
	void Blackboard::Add(const std::string& name, FrameGraphResourceHandle handle) noexcept
	{
		operator[](name) = handle;
	}

	/* �������ƻ�ȡ��Դ */
	FrameGraphResourceHandle Blackboard::GetResourceHandleInternal(const std::string& name) const noexcept
	{
		const auto it = m_NameToResourceHandleMap.find(name);
		if (it != m_NameToResourceHandleMap.end())
			return it->second;

		return {};
	}
}
