#include "Pch.h"
#include "FrameGraphPass.h"
#include "RenderPassNode.h"
#include "Wuya/Renderer/Renderer.h"
#include "Wuya/Renderer/RenderQuery.h"

namespace Wuya
{
	void IFrameGraphPass::BeforeExecute()
	{
		const char* label = m_pRenderPassNode.lock()->GetDebugName().c_str();
		Renderer::GetRenderAPI()->PushDebugGroup(label);
		RenderQueryProfiler::Instance().BeginGPUScope(label);
	}

	void IFrameGraphPass::AfterExecute()
	{
		RenderQueryProfiler::Instance().EndGPUScope();
		Renderer::GetRenderAPI()->PopDebugGroup();
	}
}
