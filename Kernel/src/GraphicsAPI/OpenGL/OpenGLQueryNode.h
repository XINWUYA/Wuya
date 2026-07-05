#pragma once
#include "Helios/VirtualDevice/DeviceQueryNode.h"

namespace Helios
{
	class OpenGLQueryNode final : public DeviceQueryNode
	{
	public:
		OpenGLQueryNode();
		~OpenGLQueryNode() override;

		void Begin() override;
		void End() override;
		bool GetQueryResult() override;
		
	private:
		uint32_t m_TimestampQueries[2] = {};
	};
}
