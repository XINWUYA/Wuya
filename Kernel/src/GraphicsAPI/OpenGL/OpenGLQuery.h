#pragma once
#include "Wuya/Renderer/RenderQuery.h"

namespace Wuya
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
