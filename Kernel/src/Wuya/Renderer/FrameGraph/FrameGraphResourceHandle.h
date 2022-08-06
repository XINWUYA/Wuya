#pragma once
namespace Wuya
{
	/* FrameGraphResourceHandle */
	class FrameGraphResourceHandle
	{
		static constexpr uint16_t UNINITIALIZED_IDX = std::numeric_limits<uint16_t>::max();

	public:
		FrameGraphResourceHandle(const FrameGraphResourceHandle&) = default;

		/* 重载操作符 */
		FrameGraphResourceHandle& operator=(const FrameGraphResourceHandle&) = default;
		operator bool() const { return IsInitialized(); }
		bool operator<(const FrameGraphResourceHandle& other) const { return m_Index < other.m_Index; }
		bool operator>(const FrameGraphResourceHandle& other) const { return m_Index > other.m_Index; }
		bool operator==(const FrameGraphResourceHandle& other) const { return m_Index == other.m_Index; }
		bool operator!=(const FrameGraphResourceHandle& other) const { return !operator==(other); }

		/* 获取Index */
		uint16_t GetIndex() const { return m_Index; }

		/* 是否已初始化过 */
		bool IsInitialized() const { return m_Index != UNINITIALIZED_IDX; }
		/* 重置 */
		void Reset();

	private:
		FrameGraphResourceHandle() noexcept = default;
		explicit FrameGraphResourceHandle(uint16_t index)
			: m_Index(index)
		{}

		/* 索引号 */
		uint16_t m_Index{ UNINITIALIZED_IDX };
		/* 版本号 */
		uint16_t m_Version{ 0 };

		friend class FrameGraph;
		friend class FramwGraphResources;
		friend class Blackboard;

		template<typename ResourceType>
		friend class FrameGraphResourceHandleTyped;
	};

	/* 指定类型的资源Handle */
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
