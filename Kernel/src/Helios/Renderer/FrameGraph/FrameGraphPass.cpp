#include "Pch.h"
#include "FrameGraphPass.h"
#include "RenderPassNode.h"
#include "Helios/Renderer/Renderer.h"
#include "Helios/Renderer/RenderQuery.h"

namespace Helios
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
