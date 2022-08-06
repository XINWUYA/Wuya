#pragma once
#include "FrameGraphResourceHandle.h"

namespace Wuya
{
	/*
	 * �������к��ռ�FrameGraph�к���Pass��Ҫ�õ�����Դhandle
	 * ��ͨ����ԴName��ȡ
	 */
	class Blackboard
	{
	public:
		Blackboard() = default;
		~Blackboard() = default;

		/* [] ���أ�������Դ��ֱ�Ӵ�ȡ */
		FrameGraphResourceHandle& operator[](const std::string& name) noexcept;
		/* ��ŵ�ӳ���� */
		void Add(const std::string& name, FrameGraphResourceHandle handle) noexcept;

		/* �������ƻ�ȡ��Դ */
		template <typename ResourceType>
		FrameGraphResourceHandleTyped<ResourceType> GetResourceHandle(const std::string& name) const noexcept
		{
			return static_cast<FrameGraphResourceHandleTyped<ResourceType>>(GetResourceHandleInternal(name));
		}

	private:
		/* �������ƻ�ȡ��Դ */
		FrameGraphResourceHandle GetResourceHandleInternal(const std::string& name) const noexcept;

		/* ��Դ������ԴHandle��ӳ��<name, FrameGraphResourceHandle> */
		std::unordered_map<std::string, FrameGraphResourceHandle> m_NameToResourceHandleMap{};
	};

}
