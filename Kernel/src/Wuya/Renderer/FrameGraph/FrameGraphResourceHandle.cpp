#include "Pch.h"
#include "FrameGraphResourceHandle.h"

namespace Wuya
{
	void FrameGraphResourceHandle::Reset()
	{
		m_Index = UNINITIALIZED_IDX;
		m_Version = 0;
	}
}
