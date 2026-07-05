#include "Pch.h"
#include "FrameGraphResourceHandle.h"

namespace Helios
{
	void FrameGraphResourceHandle::Reset()
	{
		m_Index = UNINITIALIZED_IDX;
		m_Version = 0;
	}
}
