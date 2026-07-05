#pragma once
#include "Helios/Renderer/RenderQuery.h"

namespace Helios
{
	class OpenGLQueryNode final : public RenderQueryNode
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
