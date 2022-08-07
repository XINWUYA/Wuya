#pragma once
namespace Wuya
{
	/* FrameGraphResourceHandle */
	class FrameGraphResourceHandle
	{
		static constexpr uint16_t UNINITIALIZED_IDX = std::numeric_limits<uint16_t>::max();

	public:
		FrameGraphResourceHandle(const FrameGraphResourceHandle&) = default;

		/* ���ز����� */
		FrameGraphResourceHandle& operator=(const FrameGraphResourceHandle&) = default;
		operator bool() const { return IsInitialized(); }
		bool operator<(const FrameGraphResourceHandle& other) const { return m_Index < other.m_Index; }
		bool operator>(const FrameGraphResourceHandle& other) const { return m_Index > other.m_Index; }
		bool operator==(const FrameGraphResourceHandle& other) const { return m_Index == other.m_Index; }
		bool operator!=(const FrameGraphResourceHandle& other) const { return !operator==(other); }

		/* ��ȡIndex */
		uint16_t GetIndex() const { return m_Index; }

		/* �Ƿ��ѳ�ʼ���� */
		bool IsInitialized() const { return m_Index != UNINITIALIZED_IDX; }
		/* ���� */
		void Reset();

	private:
		FrameGraphResourceHandle() noexcept = default;
		explicit FrameGraphResourceHandle(uint16_t index)
			: m_Index(index)
		{}

		/* ������ */
		uint16_t m_Index{ UNINITIALIZED_IDX };
		/* �汾�� */
		uint16_t m_Version{ 0 };

		friend class FrameGraph;
		friend class FramwGraphResources;
		friend class Blackboard;
		friend class RenderView;

		template<typename ResourceType>
		friend class FrameGraphResourceHandleTyped;
	};

	/* ָ�����͵���ԴHandle */
	template<typename ResourceType>
	class FrameGraphResourceHandleTyped : public FrameGraphResourceHandle
	{
	public:
		FrameGraphResourceHandleTyped() noexcept = default;
		explicit FrameGraphResourceHandleTyped(FrameGraphResourceHandle handle)
			: FrameGraphResourceHandle(handle)
		{}
	};
}
