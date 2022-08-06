#include "Pch.h"
#include "Blackboard.h"

namespace Wuya
{
	/* [] 重载，根据资源名直接存取 */
	FrameGraphResourceHandle& Blackboard::operator[](const std::string& name) noexcept
	{
		auto [iter, _] = m_NameToResourceHandleMap.insert_or_assign(name, FrameGraphResourceHandle{});
		return iter->second;
	}

	/* 存放到映射中 */
	void Blackboard::Add(const std::string& name, FrameGraphResourceHandle handle) noexcept
	{
		operator[](name) = handle;
	}

	/* 根据名称获取资源 */
	FrameGraphResourceHandle Blackboard::GetResourceHandleInternal(const std::string& name) const noexcept
	{
		const auto it = m_NameToResourceHandleMap.find(name);
		if (it != m_NameToResourceHandleMap.end())
			return it->second;

		return {};
	}
}
